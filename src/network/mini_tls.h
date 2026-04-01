//
//  mini_tls.h
//  最简TLS实现（零依赖）
//
//  目标：实现TLS 1.2基础功能，支持HTTPS连接
//  原则：从零开始，不依赖任何外部库
//

#ifndef MINI_TLS_H
#define MINI_TLS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// ==================== 常量定义 ====================

// TLS记录类型
#define TLS_TYPE_CHANGE_CIPHER_SPEC 20
#define TLS_TYPE_ALERT              21
#define TLS_TYPE_HANDSHAKE          22
#define TLS_TYPE_APPLICATION_DATA   23

// TLS握手类型
#define TLS_HELLO_REQUEST           0
#define TLS_CLIENT_HELLO            1
#define TLS_SERVER_HELLO            2
#define TLS_CERTIFICATE             11
#define TLS_SERVER_KEY_EXCHANGE     12
#define TLS_CERTIFICATE_REQUEST     13
#define TLS_SERVER_HELLO_DONE       14
#define TLS_CERTIFICATE_VERIFY      15
#define TLS_CLIENT_KEY_EXCHANGE     16
#define TLS_FINISHED                20

// TLS版本
#define TLS_VERSION_1_0 0x0301
#define TLS_VERSION_1_1 0x0302
#define TLS_VERSION_1_2 0x0303
#define TLS_VERSION_1_3 0x0304

// 密码套件（简化）
#define TLS_RSA_WITH_AES_128_CBC_SHA 0x002F

// ==================== 结构定义 ====================

// TLS记录头
typedef struct {
    uint8_t type;
    uint16_t version;
    uint16_t length;
} TLSRecordHeader;

// TLS握手头
typedef struct {
    uint8_t type;
    uint8_t length[3];  // 24位长度（3字节）
} TLSHandshakeHeader;

// Client Hello消息
typedef struct {
    uint16_t version;
    uint8_t random[32];
    uint8_t session_id_len;
    uint8_t session_id[32];
    uint16_t cipher_suites_len;
    uint16_t cipher_suites[10];
    uint8_t compression_methods_len;
    uint8_t compression_methods;
    uint16_t extensions_len;
} TLSClientHello;

// ==================== AES加密 ====================

// AES-128加密上下文
typedef struct {
    uint32_t round_keys[44];  // 11轮 * 4个字
} AES128Context;

// AES初始化
void aes128_init(AES128Context *ctx, const uint8_t key[16]);

// AES加密（ECB模式）
void aes128_encrypt_ecb(AES128Context *ctx, const uint8_t plaintext[16], uint8_t ciphertext[16]);

// AES解密（ECB模式）
void aes128_decrypt_ecb(AES128Context *ctx, const uint8_t ciphertext[16], uint8_t plaintext[16]);

// AES加密（CBC模式）
void aes128_encrypt_cbc(AES128Context *ctx, const uint8_t *plaintext, size_t len, 
                       const uint8_t iv[16], uint8_t *ciphertext);

// AES解密（CBC模式）
void aes128_decrypt_cbc(AES128Context *ctx, const uint8_t *ciphertext, size_t len,
                       const uint8_t iv[16], uint8_t *plaintext);

// ==================== SHA256哈希 ====================

// SHA256上下文
typedef struct {
    uint32_t state[8];
    uint64_t bitcount;
    uint8_t buffer[64];
} SHA256Context;

// SHA256初始化
void sha256_init(SHA256Context *ctx);

// SHA256更新
void sha256_update(SHA256Context *ctx, const uint8_t *data, size_t len);

// SHA256完成
void sha256_final(SHA256Context *ctx, uint8_t hash[32]);

// SHA256单次调用
void sha256(const uint8_t *data, size_t len, uint8_t hash[32]);

// ==================== HMAC ====================

// HMAC-SHA256
void hmac_sha256(const uint8_t *key, size_t key_len,
                const uint8_t *data, size_t data_len,
                uint8_t hmac[32]);

// ==================== PRF（伪随机函数） ====================

// TLS PRF（伪随机函数）
void tls_prf(const uint8_t *secret, size_t secret_len,
            const char *label, size_t label_len,
            const uint8_t *seed, size_t seed_len,
            uint8_t *output, size_t output_len);

// ==================== 测试宏 ====================

// 定义这些宏来启用测试
// #define AES_TEST
// #define SHA256_TEST

// ==================== RSA加密（简化） ====================

// RSA公钥
typedef struct {
    uint8_t modulus[256];  // n
    uint8_t exponent[3];   // e (通常0x010001)
    size_t modulus_len;
} RSAPublicKey;

// RSA加密
int rsa_encrypt(const RSAPublicKey *key, const uint8_t *plaintext, size_t plaintext_len,
               uint8_t *ciphertext, size_t ciphertext_len);

// ==================== TLS核心函数 ====================

// 创建TLS客户端上下文
void* tls_client_create(void);

// 销毁TLS客户端上下文
void tls_client_destroy(void *ctx);

// TLS握手
int tls_handshake(void *ctx, int sock, const char *hostname);

// 发送TLS加密数据
int tls_send(void *ctx, int sock, const uint8_t *data, size_t len);

// 接收TLS加密数据
int tls_recv(void *ctx, int sock, uint8_t *buffer, size_t buffer_len);

// 关闭TLS连接
void tls_close(void *ctx, int sock);

// ==================== 工具函数 ====================

// 生成随机数
void tls_random(uint8_t *buffer, size_t len);

// 计算主密钥
void compute_master_secret(const uint8_t *pre_master_secret, size_t pre_master_len,
                          const uint8_t *client_random, const uint8_t *server_random,
                          uint8_t master_secret[48]);

// 计算密钥材料
void compute_key_material(const uint8_t master_secret[48],
                         const uint8_t *client_random, const uint8_t *server_random,
                         uint8_t client_write_key[16], uint8_t server_write_key[16],
                         uint8_t client_write_iv[16], uint8_t server_write_iv[16]);

#endif /* MINI_TLS_H */