//
//  ai_tcp.c
//  纯TCP/IP AI通信实现（零依赖）
//
//  功能：
//  1. DNS解析（手动实现或使用系统调用）
//  2. TCP连接建立
//  3. HTTP/1.1请求发送
//  4. HTTPS支持（可选，需要TLS实现）
//  5. HTTP响应解析
//  6. JSON提取
//

#include "ai_integration.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

// ==================== TCP/IP系统调用 ====================

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #define close closesocket
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <fcntl.h>
#endif

// 简单的DNS解析（使用系统gethostbyname）
static char* resolve_hostname(const char *hostname) {
    struct hostent *host = gethostbyname(hostname);
    if (!host) {
        return NULL;
    }
    
    // 获取第一个IP地址
    struct in_addr **addr_list = (struct in_addr **)host->h_addr_list;
    if (addr_list[0]) {
        char *ip = inet_ntoa(*addr_list[0]);
        return strdup(ip);
    }
    
    return NULL;
}

// 从URL中提取主机名和路径
static int parse_url(const char *url, char **hostname, char **path, int *port) {
    // 默认值
    *port = 443;  // HTTPS默认端口
    *hostname = NULL;
    *path = NULL;
    
    // 跳过协议头
    const char *host_start = url;
    if (strncmp(url, "https://", 8) == 0) {
        host_start = url + 8;
        *port = 443;
    } else if (strncmp(url, "http://", 7) == 0) {
        host_start = url + 7;
        *port = 80;
    }
    
    // 查找路径开始位置
    const char *path_start = strchr(host_start, '/');
    if (path_start) {
        // 提取主机名
        size_t host_len = path_start - host_start;
        *hostname = malloc(host_len + 1);
        if (!*hostname) return 0;
        strncpy(*hostname, host_start, host_len);
        (*hostname)[host_len] = '\0';
        
        // 提取路径
        *path = strdup(path_start);
        if (!*path) {
            free(*hostname);
            return 0;
        }
    } else {
        // 没有路径，只有主机名
        *hostname = strdup(host_start);
        *path = strdup("/");
        if (!*hostname || !*path) {
            free(*hostname);
            free(*path);
            return 0;
        }
    }
    
    // 检查是否有自定义端口
    char *colon = strchr(*hostname, ':');
    if (colon) {
        *colon = '\0';  // 截断主机名
        *port = atoi(colon + 1);
    }
    
    return 1;
}

// 建立TCP连接
static int tcp_connect(const char *hostname, int port) {
    // 解析主机名
    char *ip = resolve_hostname(hostname);
    if (!ip) {
        return -1;
    }
    
    // 创建socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        free(ip);
        return -1;
    }
    
    // 设置地址结构
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        free(ip);
        close(sock);
        return -1;
    }
    
    free(ip);
    
    // 连接服务器
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(sock);
        return -1;
    }
    
    // 设置超时（可选）
    struct timeval timeout;
    timeout.tv_sec = 30;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    
    return sock;
}

// 发送HTTP请求
static int send_http_request(int sock, const char *hostname, const char *path, 
                            const char *headers, const char *body) {
    char request[8192];
    int len = 0;
    
    // 构建请求行
    len += snprintf(request + len, sizeof(request) - len, 
                   "POST %s HTTP/1.1\r\n", path);
    
    // 添加主机头
    len += snprintf(request + len, sizeof(request) - len, 
                   "Host: %s\r\n", hostname);
    
    // 添加其他头部
    if (headers) {
        len += snprintf(request + len, sizeof(request) - len, "%s", headers);
    }
    
    // 添加内容类型和长度
    len += snprintf(request + len, sizeof(request) - len, 
                   "Content-Type: application/json\r\n");
    
    if (body) {
        len += snprintf(request + len, sizeof(request) - len, 
                       "Content-Length: %zu\r\n", strlen(body));
    }
    
    // 结束头部
    len += snprintf(request + len, sizeof(request) - len, "\r\n");
    
    // 添加请求体
    if (body) {
        len += snprintf(request + len, sizeof(request) - len, "%s", body);
    }
    
    // 发送请求
    int sent = 0;
    while (sent < len) {
        int n = send(sock, request + sent, len - sent, 0);
        if (n <= 0) {
            return -1;
        }
        sent += n;
    }
    
    return sent;
}

// 接收HTTP响应
static char* receive_http_response(int sock) {
    char buffer[4096];
    char *response = NULL;
    size_t total_size = 0;
    int headers_complete = 0;
    size_t content_length = 0;
    
    while (1) {
        int n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) {
            break;
        }
        
        buffer[n] = '\0';
        
        // 扩展响应缓冲区
        char *new_response = realloc(response, total_size + n + 1);
        if (!new_response) {
            free(response);
            return NULL;
        }
        response = new_response;
        
        // 追加数据
        memcpy(response + total_size, buffer, n);
        total_size += n;
        response[total_size] = '\0';
        
        // 检查是否收到完整头部
        if (!headers_complete) {
            char *header_end = strstr(response, "\r\n\r\n");
            if (header_end) {
                headers_complete = 1;
                
                // 查找Content-Length
                char *cl_header = strstr(response, "Content-Length: ");
                if (cl_header) {
                    content_length = atoi(cl_header + 16);
                }
                
                // 如果没有Content-Length，可能是分块传输或连接关闭
                if (content_length == 0) {
                    // 简单处理：读取直到连接关闭
                    continue;
                }
            }
        }
        
        // 如果知道内容长度，检查是否接收完成
        if (headers_complete && content_length > 0) {
            // 找到正文开始位置
            char *body_start = strstr(response, "\r\n\r\n");
            if (body_start) {
                body_start += 4;
                size_t body_received = total_size - (body_start - response);
                if (body_received >= content_length) {
                    break;  // 接收完成
                }
            }
        }
    }
    
    return response;
}

// 从HTTP响应中提取正文
static char* extract_http_body(const char *response) {
    if (!response) return NULL;
    
    // 查找正文开始位置
    char *body_start = strstr(response, "\r\n\r\n");
    if (!body_start) {
        return NULL;
    }
    
    body_start += 4;  // 跳过空行
    
    // 检查是否为分块传输
    if (strstr(response, "Transfer-Encoding: chunked")) {
        // 简化处理：只提取第一个块
        char *chunk_end = strstr(body_start, "\r\n");
        if (chunk_end) {
            size_t chunk_size = strtoul(body_start, NULL, 16);
            if (chunk_size == 0) {
                return strdup("");  // 结束块
            }
            
            // 跳过块大小行
            char *chunk_data = chunk_end + 2;
            char *data_end = chunk_data + chunk_size;
            
            char *body = malloc(chunk_size + 1);
            if (!body) return NULL;
            
            strncpy(body, chunk_data, chunk_size);
            body[chunk_size] = '\0';
            return body;
        }
    }
    
    // 普通响应，直接返回正文
    return strdup(body_start);
}

// ==================== AI通信主函数 ====================

// 纯TCP/IP AI请求
char* ai_tcp_request(AIContext *context, const char *prompt) {
    if (!context || !context->config.configured) {
        return strdup("错误：AI模型未配置");
    }
    
    // 解析URL
    char *hostname = NULL;
    char *path = NULL;
    int port = 0;
    
    if (!parse_url(context->config.api_url, &hostname, &path, &port)) {
        return strdup("错误：无法解析API地址");
    }
    
    // 构建请求JSON
    char *json_body = NULL;
    asprintf(&json_body,
        "{\n"
        "  \"model\": \"%s\",\n"
        "  \"messages\": [\n"
        "    {\"role\": \"system\", \"content\": \"你是一个专业的二进制文件分析助手。\"},\n"
        "    {\"role\": \"user\", \"content\": \"%s\"}\n"
        "  ],\n"
        "  \"max_tokens\": %d,\n"
        "  \"temperature\": %.1f\n"
        "}",
        context->config.model_name ? context->config.model_name : "gpt-4",
        prompt,
        context->config.max_tokens,
        context->config.temperature);
    
    if (!json_body) {
        free(hostname);
        free(path);
        return strdup("错误：无法构建请求数据");
    }
    
    // 构建认证头部
    char *auth_header = NULL;
    if (context->config.api_key) {
        asprintf(&auth_header, "Authorization: Bearer %s\r\n", context->config.api_key);
    }
    
    // 建立TCP连接
    int sock = tcp_connect(hostname, port);
    if (sock < 0) {
        char *error = NULL;
        asprintf(&error, "错误：无法连接到服务器 %s:%d\n"
                        "请检查:\n"
                        "1. 网络连接\n"
                        "2. 主机名解析\n"
                        "3. 防火墙设置",
                        hostname, port);
        free(hostname);
        free(path);
        free(json_body);
        free(auth_header);
        return error;
    }
    
    // 发送HTTP请求
    if (send_http_request(sock, hostname, path, auth_header, json_body) < 0) {
        char *error = NULL;
        asprintf(&error, "错误：发送请求失败");
        close(sock);
        free(hostname);
        free(path);
        free(json_body);
        free(auth_header);
        return error;
    }
    
    // 接收响应
    char *http_response = receive_http_response(sock);
    close(sock);
    
    // 清理
    free(hostname);
    free(path);
    free(json_body);
    free(auth_header);
    
    if (!http_response) {
        return strdup("错误：接收响应失败");
    }
    
    // 检查HTTP状态码
    int http_status = 0;
    if (sscanf(http_response, "HTTP/1.1 %d", &http_status) == 1) {
        if (http_status != 200) {
            char *error = NULL;
            asprintf(&error, "API错误 (HTTP %d)", http_status);
            free(http_response);
            return error;
        }
    }
    
    // 提取正文
    char *body = extract_http_body(http_response);
    free(http_response);
    
    if (!body) {
        return strdup("错误：无法提取响应正文");
    }
    
    // 从JSON中提取AI回复（复用之前的函数）
    char *ai_response = extract_ai_response(body);
    free(body);
    
    if (!ai_response) {
        return strdup("错误：无法解析AI响应");
    }
    
    return ai_response;
}

