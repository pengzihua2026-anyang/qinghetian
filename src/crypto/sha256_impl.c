//
//  sha256_impl.c
//  纯C语言SHA256实现（零依赖）
//
//  参考：FIPS 180-4标准 + GitHub开源实现
//  特点：完全自主实现，不依赖任何外部库
//

#include "mini_tls.h"
#include <string.h>

// ==================== SHA256常量 ====================

// 初始哈希值（前8个质数的平方根小数部分前32位）
static const uint32_t H0[8] = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

// 常量K（前64个质数的立方根小数部分前32位）
static const uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

// ==================== 工具函数 ====================

// 循环右移
static inline uint32_t rotr(uint32_t x, int n) {
    return (x >> n) | (x << (32 - n));
}

// 循环左移
static inline uint32_t rotl(uint32_t x, int n) {
    return (x << n) | (x >> (32 - n));
}

// SHA256函数定义
static inline uint32_t ch(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) ^ (~x & z);
}

static inline uint32_t maj(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

static inline uint32_t sigma0(uint32_t x) {
    return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

static inline uint32_t sigma1(uint32_t x) {
    return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

static inline uint32_t gamma0(uint32_t x) {
    return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

static inline uint32_t gamma1(uint32_t x) {
    return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

// ==================== SHA256核心 ====================

// SHA256初始化
void sha256_init(SHA256Context *ctx) {
    memcpy(ctx->state, H0, sizeof(H0));
    ctx->bitcount = 0;
    memset(ctx->buffer, 0, sizeof(ctx->buffer));
}

// 处理一个512位块
static void sha256_transform(SHA256Context *ctx, const uint8_t data[64]) {
    uint32_t a, b, c, d, e, f, g, h;
    uint32_t w[64];
    int i;
    
    // 准备消息调度表（前16个字）
    for (i = 0; i < 16; i++) {
        w[i] = ((uint32_t)data[4*i] << 24) |
               ((uint32_t)data[4*i+1] << 16) |
               ((uint32_t)data[4*i+2] << 8) |
               ((uint32_t)data[4*i+3]);
    }
    
    // 扩展消息调度表（后48个字）
    for (i = 16; i < 64; i++) {
        w[i] = gamma1(w[i-2]) + w[i-7] + gamma0(w[i-15]) + w[i-16];
    }
    
    // 初始化工作变量
    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];
    f = ctx->state[5];
    g = ctx->state[6];
    h = ctx->state[7];
    
    // 主循环
    for (i = 0; i < 64; i++) {
        uint32_t t1 = h + sigma1(e) + ch(e, f, g) + K[i] + w[i];
        uint32_t t2 = sigma0(a) + maj(a, b, c);
        
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }
    
    // 更新状态
    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
    ctx->state[5] += f;
    ctx->state[6] += g;
    ctx->state[7] += h;
}

// SHA256更新
void sha256_update(SHA256Context *ctx, const uint8_t *data, size_t len) {
    size_t index = (size_t)(ctx->bitcount >> 3) & 0x3F;
    
    // 更新位计数
    ctx->bitcount += (uint64_t)len << 3;
    
    // 处理缓冲区中的部分数据
    size_t part_len = 64 - index;
    size_t i = 0;
    
    if (len >= part_len) {
        memcpy(&ctx->buffer[index], data, part_len);
        sha256_transform(ctx, ctx->buffer);
        
        for (i = part_len; i + 63 < len; i += 64) {
            sha256_transform(ctx, &data[i]);
        }
        
        index = 0;
    }
    
    // 缓冲剩余数据
    if (i < len) {
        memcpy(&ctx->buffer[index], &data[i], len - i);
    }
}

// SHA256完成
void sha256_final(SHA256Context *ctx, uint8_t hash[32]) {
    size_t index = (size_t)(ctx->bitcount >> 3) & 0x3F;
    
    // 填充：先添加一个1位
    ctx->buffer[index++] = 0x80;
    
    // 如果空间不足，处理当前块
    if (index > 56) {
        memset(&ctx->buffer[index], 0, 64 - index);
        sha256_transform(ctx, ctx->buffer);
        memset(ctx->buffer, 0, 56);
    } else {
        memset(&ctx->buffer[index], 0, 56 - index);
    }
    
    // 添加位长度（大端序）
    uint64_t bitcount = ctx->bitcount;
    ctx->buffer[63] = (uint8_t)(bitcount);
    ctx->buffer[62] = (uint8_t)(bitcount >> 8);
    ctx->buffer[61] = (uint8_t)(bitcount >> 16);
    ctx->buffer[60] = (uint8_t)(bitcount >> 24);
    ctx->buffer[59] = (uint8_t)(bitcount >> 32);
    ctx->buffer[58] = (uint8_t)(bitcount >> 40);
    ctx->buffer[57] = (uint8_t)(bitcount >> 48);
    ctx->buffer[56] = (uint8_t)(bitcount >> 56);
    
    // 处理最后一个块
    sha256_transform(ctx, ctx->buffer);
    
    // 输出哈希值（大端序）
    for (int i = 0; i < 8; i++) {
        hash[4*i] = (uint8_t)(ctx->state[i] >> 24);
        hash[4*i+1] = (uint8_t)(ctx->state[i] >> 16);
        hash[4*i+2] = (uint8_t)(ctx->state[i] >> 8);
        hash[4*i+3] = (uint8_t)(ctx->state[i]);
    }
}

// SHA256单次调用
void sha256(const uint8_t *data, size_t len, uint8_t hash[32]) {
    SHA256Context ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, data, len);
    sha256_final(&ctx, hash);
}

// ==================== HMAC-SHA256 ====================

void hmac_sha256(const uint8_t *key, size_t key_len,
                const uint8_t *data, size_t data_len,
                uint8_t hmac[32]) {
    SHA256Context ctx;
    uint8_t key_block[64];
    uint8_t inner_key[64];
    uint8_t outer_key[64];
    uint8_t inner_hash[32];
    
    // 如果密钥长度超过64字节，先哈希
    if (key_len > 64) {
        sha256(key, key_len, key_block);
        key_len = 32;
        key = key_block;
    }
    
    // 准备内部和外部密钥
    memset(inner_key, 0x36, 64);
    memset(outer_key, 0x5c, 64);
    
    for (size_t i = 0; i < key_len; i++) {
        inner_key[i] ^= key[i];
        outer_key[i] ^= key[i];
    }
    
    // 内部哈希：inner_key || data
    sha256_init(&ctx);
    sha256_update(&ctx, inner_key, 64);
    sha256_update(&ctx, data, data_len);
    sha256_final(&ctx, inner_hash);
    
    // 外部哈希：outer_key || inner_hash
    sha256_init(&ctx);
    sha256_update(&ctx, outer_key, 64);
    sha256_update(&ctx, inner_hash, 32);
    sha256_final(&ctx, hmac);
}

// ==================== 测试函数 ====================

#ifdef SHA256_TEST
#include <stdio.h>

// 打印十六进制
static void print_hex(const char *label, const uint8_t *data, size_t len) {
    printf("%s: ", label);
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

// 测试SHA256
void test_sha256(void) {
    printf("=== SHA256测试 ===\n\n");
    
    // 测试用例1：空字符串
    printf("1. 空字符串测试:\n");
    uint8_t hash1[32];
    sha256((const uint8_t*)"", 0, hash1);
    print_hex("期望", (const uint8_t*)"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", 64);
    print_hex("实际", hash1, 32);
    
    // 测试用例2："abc"
    printf("\n2. \"abc\"测试:\n");
    uint8_t hash2[32];
    sha256((const uint8_t*)"abc", 3, hash2);
    print_hex("期望", (const uint8_t*)"ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad", 64);
    print_hex("实际", hash2, 32);
    
    // 测试用例3："hello world"
    printf("\n3. \"hello world\"测试:\n");
    uint8_t hash3[32];
    sha256((const uint8_t*)"hello world", 11, hash3);
    print_hex("期望", (const uint8_t*)"b94d27b9934d3e08a52e52d7da7dabfac484efe37a5380ee9088f7ace2efcde9", 64);
    print_hex("实际", hash3, 32);
    
    // 测试HMAC-SHA256
    printf("\n4. HMAC-SHA256测试:\n");
    uint8_t hmac[32];
    const uint8_t key[] = "key";
    const uint8_t data[] = "The quick brown fox jumps over the lazy dog";
    
    hmac_sha256(key, 3, data, strlen((const char*)data), hmac);
    print_hex("HMAC", hmac, 32);
    
    printf("\n=== SHA256测试完成 ===\n");
}

int main() {
    test_sha256();
    return 0;
}
#endif