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
//  simple_tls.c
//  最简TLS实现（阶段1：基础握手）
//
//  目标：实现最基本的TLS 1.2握手，支持HTTPS连接
//  特点：预共享密钥，跳过证书验证，仅用于测试
//

#include "mini_tls.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
#endif

// ==================== 预共享密钥（测试用） ====================

// 测试用的预共享主密钥（实际应该通过握手协商）
static const uint8_t TEST_MASTER_SECRET[48] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f
};

// 测试用的客户端随机数
static const uint8_t TEST_CLIENT_RANDOM[32] = {
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
};

// 测试用的服务器随机数
static const uint8_t TEST_SERVER_RANDOM[32] = {
    0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88,
    0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00,
    0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88,
    0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00
};

// ==================== TLS上下文 ====================

typedef struct {
    int sock;                       // 底层socket
    uint8_t client_random[32];      // 客户端随机数
    uint8_t server_random[32];      // 服务器随机数
    uint8_t master_secret[48];      // 主密钥
    uint8_t client_write_key[16];   // 客户端写密钥
    uint8_t server_write_key[16];   // 服务器写密钥
    uint8_t client_write_iv[16];    // 客户端写IV
    uint8_t server_write_iv[16];    // 服务器写IV
    uint8_t sequence_number[8];     // 序列号
    int is_connected;               // 是否已连接
} SimpleTLSContext;

// ==================== 工具函数 ====================

// 生成随机数（简化版：使用时间+计数器）
static void generate_random(uint8_t *buffer, size_t len) {
    static uint32_t counter = 0;
    time_t t = time(NULL);
    
    for (size_t i = 0; i < len; i++) {
        buffer[i] = ((t >> (i * 8)) & 0xFF) ^ (counter++ & 0xFF);
    }
}

// 简单的XOR加密（临时使用，待替换为AES）
static void xor_encrypt(const uint8_t *key, size_t key_len,
                       const uint8_t *input, size_t input_len,
                       uint8_t *output) {
    for (size_t i = 0; i < input_len; i++) {
        output[i] = input[i] ^ key[i % key_len];
    }
}

// 简单的XOR解密（与加密相同）
static void xor_decrypt(const uint8_t *key, size_t key_len,
                       const uint8_t *input, size_t input_len,
                       uint8_t *output) {
    xor_encrypt(key, key_len, input, input_len, output);
}

// 计算伪随机密钥材料（简化版）
static void compute_test_keys(void) {
    // 在实际TLS中，这里应该使用PRF函数
    // 这里使用简单的测试密钥
}

// ==================== TLS记录层 ====================

// 发送TLS记录
static int send_tls_record(SimpleTLSContext *ctx, uint8_t type,
                          const uint8_t *data, size_t data_len) {
    // TLS记录头：类型(1) + 版本(2) + 长度(2)
    uint8_t header[5];
    header[0] = type;                          // 记录类型
    header[1] = 0x03;                          // TLS 1.2主版本
    header[2] = 0x03;                          // TLS 1.2次版本
    header[3] = (data_len >> 8) & 0xFF;        // 长度高字节
    header[4] = data_len & 0xFF;               // 长度低字节
    
    // 发送记录头
    if (send(ctx->sock, header, sizeof(header), 0) != sizeof(header)) {
        return -1;
    }
    
    // 发送记录数据
    if (send(ctx->sock, data, data_len, 0) != data_len) {
        return -1;
    }
    
    return 0;
}

// 接收TLS记录
static int recv_tls_record(SimpleTLSContext *ctx, uint8_t *type,
                          uint8_t *buffer, size_t buffer_size) {
    uint8_t header[5];
    
    // 接收记录头
    if (recv(ctx->sock, header, sizeof(header), 0) != sizeof(header)) {
        return -1;
    }
    
    *type = header[0];
    uint16_t length = (header[3] << 8) | header[4];
    
    if (length > buffer_size) {
        return -1;  // 缓冲区太小
    }
    
    // 接收记录数据
    if (recv(ctx->sock, buffer, length, 0) != length) {
        return -1;
    }
    
    return length;
}

// ==================== 最简TLS握手 ====================

// 发送Client Hello
static int send_client_hello(SimpleTLSContext *ctx) {
    // Client Hello消息结构（简化版）
    uint8_t client_hello[128];
    size_t pos = 0;
    
    // 握手头：类型(1) + 长度(3)
    client_hello[pos++] = TLS_CLIENT_HELLO;    // 握手类型
    client_hello[pos++] = 0x00;                // 长度（24位）
    client_hello[pos++] = 0x00;
    client_hello[pos++] = 0x40;                // 总长度64字节
    
    // 协议版本：TLS 1.2
    client_hello[pos++] = 0x03;  // 主版本
    client_hello[pos++] = 0x03;  // 次版本
    
    // 随机数（32字节）
    generate_random(ctx->client_random, 32);
    memcpy(client_hello + pos, ctx->client_random, 32);
    pos += 32;
    
    // 会话ID长度：0（新会话）
    client_hello[pos++] = 0x00;
    
    // 密码套件：只支持TLS_RSA_WITH_AES_128_CBC_SHA
    client_hello[pos++] = 0x00;  // 长度高字节
    client_hello[pos++] = 0x02;  // 长度低字节（2字节）
    client_hello[pos++] = 0x00;  // 套件高字节
    client_hello[pos++] = 0x2F;  // 套件低字节
    
    // 压缩方法：只支持null压缩
    client_hello[pos++] = 0x01;  // 长度
    client_hello[pos++] = 0x00;  // null压缩
    
    // 扩展长度：0（无扩展）
    client_hello[pos++] = 0x00;
    client_hello[pos++] = 0x00;
    
    // 发送Client Hello
    return send_tls_record(ctx, TLS_TYPE_HANDSHAKE, client_hello, pos);
}

// 接收Server Hello（简化版：跳过证书验证）
static int recv_server_hello(SimpleTLSContext *ctx) {
    uint8_t record_type;
    uint8_t buffer[1024];
    
    // 接收Server Hello记录
    int len = recv_tls_record(ctx, &record_type, buffer, sizeof(buffer));
    if (len < 0 || record_type != TLS_TYPE_HANDSHAKE) {
        return -1;
    }
    
    // 跳过Server Hello解析（简化版）
    // 在实际实现中，这里应该解析服务器随机数、密码套件等
    
    // 使用测试随机数
    memcpy(ctx->server_random, TEST_SERVER_RANDOM, 32);
    
    // 使用测试主密钥
    memcpy(ctx->master_secret, TEST_MASTER_SECRET, 48);
    
    return 0;
}

// 发送Client Key Exchange（简化版：预共享密钥）
static int send_client_key_exchange(SimpleTLSContext *ctx) {
    uint8_t key_exchange[64];
    size_t pos = 0;
    
    // 握手头
    key_exchange[pos++] = TLS_CLIENT_KEY_EXCHANGE;
    key_exchange[pos++] = 0x00;
    key_exchange[pos++] = 0x00;
    key_exchange[pos++] = 0x30;  // 48字节
    
    // 预共享密钥标识（简化版）
    key_exchange[pos++] = 0x50;  // 'P'
    key_exchange[pos++] = 0x53;  // 'S'
    key_exchange[pos++] = 0x4B;  // 'K'
    
    // 发送
    return send_tls_record(ctx, TLS_TYPE_HANDSHAKE, key_exchange, pos);
}

// 发送Change Cipher Spec
static int send_change_cipher_spec(SimpleTLSContext *ctx) {
    uint8_t ccs[1] = {0x01};  // Change Cipher Spec消息
    
    return send_tls_record(ctx, TLS_TYPE_CHANGE_CIPHER_SPEC, ccs, 1);
}

// 发送Finished
static int send_finished(SimpleTLSContext *ctx) {
    uint8_t finished[64];
    size_t pos = 0;
    
    // 握手头
    finished[pos++] = TLS_FINISHED;
    finished[pos++] = 0x00;
    finished[pos++] = 0x00;
    finished[pos++] = 0x0C;  // 12字节
    
    // Finished数据（简化版：固定值）
    memcpy(finished + pos, "FINISHED1234", 12);
    pos += 12;
    
    return send_tls_record(ctx, TLS_TYPE_HANDSHAKE, finished, pos);
}

// ==================== 公开API ====================

// 创建TLS上下文
void* tls_client_create(void) {
    SimpleTLSContext *ctx = calloc(1, sizeof(SimpleTLSContext));
    if (!ctx) return NULL;
    
    ctx->sock = -1;
    ctx->is_connected = 0;
    memset(ctx->sequence_number, 0, sizeof(ctx->sequence_number));
    
    return ctx;
}

// 销毁TLS上下文
void tls_client_destroy(void *tls_ctx) {
    if (tls_ctx) {
        free(tls_ctx);
    }
}

// TLS握手
int tls_handshake(void *tls_ctx, int sock, const char *hostname) {
    SimpleTLSContext *ctx = (SimpleTLSContext*)tls_ctx;
    if (!ctx) return -1;
    
    ctx->sock = sock;
    
    printf("开始TLS握手（最简版）...\n");
    
    // 1. 发送Client Hello
    printf("发送Client Hello...\n");
    if (send_client_hello(ctx) < 0) {
        printf("发送Client Hello失败\n");
        return -1;
    }
    
    // 2. 接收Server Hello（简化版）
    printf("接收Server Hello...\n");
    if (recv_server_hello(ctx) < 0) {
        printf("接收Server Hello失败\n");
        return -1;
    }
    
    // 3. 发送Client Key Exchange（简化版）
    printf("发送Client Key Exchange...\n");
    if (send_client_key_exchange(ctx) < 0) {
        printf("发送Client Key Exchange失败\n");
        return -1;
    }
    
    // 4. 发送Change Cipher Spec
    printf("发送Change Cipher Spec...\n");
    if (send_change_cipher_spec(ctx) < 0) {
        printf("发送Change Cipher Spec失败\n");
        return -1;
    }
    
    // 5. 发送Finished
    printf("发送Finished...\n");
    if (send_finished(ctx) < 0) {
        printf("发送Finished失败\n");
        return -1;
    }
    
    // 6. 接收Finished（简化版：跳过）
    printf("接收Finished（跳过）...\n");
    
    ctx->is_connected = 1;
    printf("TLS握手完成（最简版）\n");
    
    return 0;
}

// 发送TLS加密数据
int tls_send(void *tls_ctx, int sock, const uint8_t *data, size_t len) {
    SimpleTLSContext *ctx = (SimpleTLSContext*)tls_ctx;
    if (!ctx || !ctx->is_connected) return -1;
    
    // 加密数据（简化版：XOR加密）
    uint8_t encrypted[4096];
    xor_encrypt(ctx->client_write_key, 16, data, len, encrypted);
    
    // 发送TLS应用数据记录
    return send_tls_record(ctx, TLS_TYPE_APPLICATION_DATA, encrypted, len);
}

// 接收TLS加密数据
int tls_recv(void *tls_ctx, int sock, uint8_t *buffer, size_t buffer_len) {
    SimpleTLSContext *ctx = (SimpleTLSContext*)tls_ctx;
    if (!ctx || !ctx->is_connected) return -1;
    
    uint8_t record_type;
    uint8_t encrypted[4096];
    
    // 接收TLS记录
    int len = recv_tls_record(ctx, &record_type, encrypted, sizeof(encrypted));
    if (len < 0) return -1;
    
    if (record_type != TLS_TYPE_APPLICATION_DATA) {
        return -1;  // 不是应用数据
    }
    
    // 解密数据（简化版：XOR解密）
    xor_decrypt(ctx->server_write_key, 16, encrypted, len, buffer);
    
    return len;
}

// 关闭TLS连接
void tls_close(void *tls_ctx, int sock) {
    SimpleTLSContext *ctx = (SimpleTLSContext*)tls_ctx;
    if (ctx) {
        ctx->is_connected = 0;
    }
}