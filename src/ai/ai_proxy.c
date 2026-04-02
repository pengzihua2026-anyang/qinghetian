/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 安阳子涵软件开发有限公司
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
//
//  ai_proxy.c
//  HTTP代理模式实现
//
//  功能：通过本地HTTP代理转发HTTPS请求
//  原理：我们的代码 → HTTP → 本地代理 → HTTPS → AI API
//

#include "ai_integration.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

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

// ==================== 配置 ====================

// 本地代理服务器配置
#define PROXY_HOST "127.0.0.1"
#define PROXY_PORT 8080
#define PROXY_PATH "/proxy"

// ==================== TCP工具函数 ====================

// 解析主机名
static char* resolve_hostname(const char *hostname) {
    struct hostent *host = gethostbyname(hostname);
    if (!host) {
        return NULL;
    }
    
    struct in_addr **addr_list = (struct in_addr **)host->h_addr_list;
    if (addr_list[0]) {
        char *ip = inet_ntoa(*addr_list[0]);
        return strdup(ip);
    }
    
    return NULL;
}

// 建立TCP连接
static int tcp_connect(const char *hostname, int port) {
    char *ip = resolve_hostname(hostname);
    if (!ip) {
        return -1;
    }
    
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        free(ip);
        return -1;
    }
    
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
    
    // 设置超时
    struct timeval timeout;
    timeout.tv_sec = 30;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(sock);
        return -1;
    }
    
    return sock;
}

// 发送数据
static int send_all(int sock, const char *data, size_t len) {
    size_t sent = 0;
    while (sent < len) {
        int n = send(sock, data + sent, len - sent, 0);
        if (n <= 0) {
            printf("[DEBUG] receive_all: recv返回 %d\n", n);
            return -1;
        }
        sent += n;
    }
    return sent;
}

// 接收数据直到连接关闭
static char* receive_all(int sock) {
    printf("[DEBUG] receive_all: 开始接收 (socket=%d)\n", sock);
    char buffer[4096];
    char *response = NULL;
    size_t total_size = 0;
    
    while (1) {
        int n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) {
            printf("[DEBUG] receive_all: recv返回 %d, 总共收到 %zu 字节\n", n, total_size);
            break;
        }
        
        buffer[n] = '\0';
        printf("[DEBUG] receive_all: 收到 %d 字节, 预览: %.50s\n", n, buffer);
        
        char *new_response = realloc(response, total_size + n + 1);
        if (!new_response) {
            free(response);
            return NULL;
        }
        response = new_response;
        
        memcpy(response + total_size, buffer, n);
        total_size += n;
        response[total_size] = '\0';
    }
    
    printf("[DEBUG] receive_all: 返回 %zu 字节数据\n", total_size);
    if (response && total_size > 0) {
        printf("[DEBUG] 响应预览: %.200s\n", response);
    } else {
        printf("[DEBUG] receive_all: 无响应数据\n");
    }
    return response;
}

// ==================== 代理请求构建 ====================

// 构建代理请求
static char* build_proxy_request(const char *target_url, const char *api_key, 
                                const char *model, const char *prompt, 
                                int max_tokens, double temperature) {
    // 构建原始JSON请求
    char *json_body = NULL;
    asprintf(&json_body,
        "{\n"
        "  \"model\": \"%s\",\n"
        "  \"messages\": [\n"
        "    {\"role\": \"system\", \"content\": \"你是一个专业的二进制文件分析助手，擅长分析、编辑和操作二进制文件。\"},\n"
        "    {\"role\": \"user\", \"content\": \"%s\"}\n"
        "  ],\n"
        "  \"max_tokens\": %d,\n"
        "  \"temperature\": %.1f\n"
        "}",
        model, prompt, max_tokens, temperature);
    
    if (!json_body) {
        return NULL;
    }
    
    // 构建代理请求JSON
    char *proxy_request = NULL;
    asprintf(&proxy_request,
        "{\n"
        "  \"method\": \"POST\",\n"
        "  \"url\": \"%s\",\n"
        "  \"headers\": {\n"
        "    \"Content-Type\": \"application/json\",\n"
        "    \"Authorization\": \"Bearer %s\"\n"
        "  },\n"
        "  \"body\": %s\n"
        "}",
        target_url, api_key, json_body);
    
    free(json_body);
    return proxy_request;
}

// ==================== 主代理函数 ====================

// 通过代理发送AI请求
char* ai_proxy_request(AIContext *context, const char *prompt) {
    if (!context || !context->config.configured) {
        return strdup("错误：AI模型未配置");
    }
    
    // 检查是否需要代理
    int use_proxy = 0;
    const char *target_url = context->config.api_url;
    
    // 如果是HTTPS地址，使用代理
    if (strncmp(target_url, "https://", 8) == 0) {
        use_proxy = 1;
        printf("检测到HTTPS地址，使用代理模式\n");
    }
    
    // 构建代理请求
    char *proxy_json = build_proxy_request(
        target_url,
        context->config.api_key ? context->config.api_key : "",
        context->config.model_name ? context->config.model_name : "gpt-4",
        prompt,
        context->config.max_tokens,
        context->config.temperature);
    
    if (!proxy_json) {
        return strdup("错误：无法构建代理请求");
    }
    
    // 连接代理服务器
    int sock = tcp_connect(PROXY_HOST, PROXY_PORT);
    if (sock < 0) {
        char *error = NULL;
        asprintf(&error, 
            "错误：无法连接到代理服务器 %s:%d\n"
            "请先启动代理服务器：\n"
            "  python3 -m http.server %d &\n"
            "或使用其他HTTP代理",
            PROXY_HOST, PROXY_PORT, PROXY_PORT);
        free(proxy_json);
        return error;
    }
    
    // 构建HTTP请求
    char http_request[8192];
    int len = snprintf(http_request, sizeof(http_request),
        "POST %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        PROXY_PATH, PROXY_HOST, strlen(proxy_json), proxy_json);
    
    free(proxy_json);
    
    // 发送请求
    if (send_all(sock, http_request, len) < 0) {
        close(sock);
        return strdup("错误：发送代理请求失败");
    }
    
    // 接收响应
    char *http_response = receive_all(sock);
    close(sock);
    
    if (!http_response) {
        return strdup("错误：接收代理响应失败");
    }
    
    // 检查HTTP状态码（更灵活的解析）
    int http_status = 0;
    char *status_line = http_response;
    
    // 查找HTTP状态码
    if (strncmp(status_line, "HTTP/", 5) == 0) {
        // 跳过 "HTTP/"
        status_line += 5;
        
        // 跳过版本号（如 "1.1"）
        while (*status_line && *status_line != ' ') status_line++;
        while (*status_line && *status_line == ' ') status_line++;
        
        // 解析状态码
        if (sscanf(status_line, "%d", &http_status) == 1) {
            printf("[DEBUG] HTTP状态码: %d\n", http_status);
            
            if (http_status != 200) {
                char *error = NULL;
                asprintf(&error, "代理错误 (HTTP %d)", http_status);
                free(http_response);
                return error;
            }
        } else {
            printf("[DEBUG] 无法解析HTTP状态码\n");
        }
    } else {
        printf("[DEBUG] 无效的HTTP响应格式\n");
    }
    
    // 提取正文（跳过HTTP头部）
    char *body_start = strstr(http_response, "\r\n\r\n");
    if (!body_start) {
        free(http_response);
        return strdup("错误：无效的代理响应格式");
    }
    
    body_start += 4;
    char *body = strdup(body_start);
    free(http_response);
    
    if (!body) {
        return strdup("错误：无法提取响应正文");
    }
    
    // 从JSON中提取AI回复
    char *ai_response = extract_ai_response(body);
    free(body);
    
    if (!ai_response) {
        return strdup("错误：无法解析AI响应");
    }
    
    return ai_response;
}

// ==================== 备用方案：直接TCP（HTTP） ====================

// 直接TCP请求（仅限HTTP）
char* ai_direct_tcp_request(AIContext *context, const char *prompt) {
    if (!context || !context->config.configured) {
        return strdup("错误：AI模型未配置");
    }
    
    // 检查是否为HTTP地址
    const char *url = context->config.api_url;
    if (strncmp(url, "http://", 7) != 0) {
        return strdup("错误：只支持HTTP地址，请使用代理模式或配置HTTP API");
    }
    
    // 解析URL
    const char *host_start = url + 7;
    const char *path_start = strchr(host_start, '/');
    
    char hostname[256];
    char path[512];
    
    if (path_start) {
        size_t host_len = path_start - host_start;
        strncpy(hostname, host_start, host_len);
        hostname[host_len] = '\0';
        strcpy(path, path_start);
    } else {
        strcpy(hostname, host_start);
        strcpy(path, "/");
    }
    
    int port = 80;
    char *colon = strchr(hostname, ':');
    if (colon) {
        *colon = '\0';
        port = atoi(colon + 1);
    }
    
    // 构建JSON请求
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
        return strdup("错误：无法构建请求数据");
    }
    
    // 建立连接
    int sock = tcp_connect(hostname, port);
    if (sock < 0) {
        char *error = NULL;
        asprintf(&error, "错误：无法连接到 %s:%d", hostname, port);
        free(json_body);
        return error;
    }
    
    // 构建HTTP请求
    char http_request[8192];
    int len = snprintf(http_request, sizeof(http_request),
        "POST %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Type: application/json\r\n"
        "Authorization: Bearer %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        path, hostname, 
        context->config.api_key ? context->config.api_key : "",
        strlen(json_body), json_body);
    
    free(json_body);
    
    // 发送请求
    if (send_all(sock, http_request, len) < 0) {
        close(sock);
        return strdup("错误：发送请求失败");
    }
    
    // 接收响应
    char *http_response = receive_all(sock);
    close(sock);
    
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
    char *body_start = strstr(http_response, "\r\n\r\n");
    if (!body_start) {
        free(http_response);
        return strdup("错误：无效的响应格式");
    }
    
    body_start += 4;
    char *body = strdup(body_start);
    free(http_response);
    
    if (!body) {
        return strdup("错误：无法提取响应正文");
    }
    
    // 提取AI回复
    char *ai_response = extract_ai_response(body);
    free(body);
    
    if (!ai_response) {
        return strdup("错误：无法解析AI响应");
    }
    
    return ai_response;
}

// ==================== 统一入口 ====================

// 智能选择请求方式
char* ai_smart_request(AIContext *context, const char *prompt) {
    if (!context || !context->config.configured) {
        return strdup("错误：AI模型未配置");
    }
    
    const char *url = context->config.api_url;
    
    // 根据URL协议选择方式
    if (strncmp(url, "https://", 8) == 0) {
        printf("使用代理模式访问HTTPS API\n");
        return ai_proxy_request(context, prompt);
    } else if (strncmp(url, "http://", 7) == 0) {
        printf("使用直接TCP访问HTTP API\n");
        return ai_direct_tcp_request(context, prompt);
    } else {
        char *error = NULL;
        asprintf(&error, "错误：不支持的协议，请使用 http:// 或 https:// 开头");
        return error;
    }
}

// 替换原来的ai_send_request函数
char* ai_send_request(AIContext *context, const char *prompt) {
    return ai_smart_request(context, prompt);
}