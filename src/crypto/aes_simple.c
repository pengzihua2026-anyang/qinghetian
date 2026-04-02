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
//  aes_simple.c
//  简化版AES-128实现（零依赖）
//
//  参考：GitHub开源实现 + FIPS-197标准
//

#include "mini_tls.h"
#include <string.h>

// ==================== AES常量 ====================

// S盒（字节替换表）
static const uint8_t sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

// 轮常数
static const uint32_t rcon[10] = {
    0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000,
    0x20000000, 0x40000000, 0x80000000, 0x1b000000, 0x36000000
};

// ==================== 工具函数 ====================

// 字节替换
static inline uint8_t sub_byte(uint8_t b) {
    return sbox[b];
}

// 字循环左移
static inline uint32_t rot_word(uint32_t word) {
    return (word << 8) | (word >> 24);
}

// 字替换
static inline uint32_t sub_word(uint32_t word) {
    return ((uint32_t)sub_byte((word >> 24) & 0xFF) << 24) |
           ((uint32_t)sub_byte((word >> 16) & 0xFF) << 16) |
           ((uint32_t)sub_byte((word >> 8) & 0xFF) << 8) |
           ((uint32_t)sub_byte(word & 0xFF));
}

// ==================== 密钥扩展 ====================

void aes128_init(AES128Context *ctx, const uint8_t key[16]) {
    uint32_t *w = ctx->round_keys;
    
    // 复制初始密钥
    for (int i = 0; i < 4; i++) {
        w[i] = ((uint32_t)key[4*i] << 24) |
               ((uint32_t)key[4*i+1] << 16) |
               ((uint32_t)key[4*i+2] << 8) |
               ((uint32_t)key[4*i+3]);
    }
    
    // 扩展密钥
    for (int i = 4; i < 44; i++) {
        uint32_t temp = w[i-1];
        
        if (i % 4 == 0) {
            temp = sub_word(rot_word(temp)) ^ rcon[i/4 - 1];
        }
        
        w[i] = w[i-4] ^ temp;
    }
}

// ==================== AES核心 ====================

// 简化版AES加密（ECB模式）
void aes128_encrypt_ecb(AES128Context *ctx, const uint8_t plaintext[16], uint8_t ciphertext[16]) {
    // 简化实现：直接使用XOR（待完善）
    // 实际应该实现完整的10轮AES加密
    for (int i = 0; i < 16; i++) {
        ciphertext[i] = plaintext[i] ^ ctx->round_keys[i/4] >> (24 - (i%4)*8);
    }
}

// 简化版AES解密（ECB模式）
void aes128_decrypt_ecb(AES128Context *ctx, const uint8_t ciphertext[16], uint8_t plaintext[16]) {
    // 简化实现：直接使用XOR（待完善）
    aes128_encrypt_ecb(ctx, ciphertext, plaintext);  // 对称加密
}

// CBC模式加密
void aes128_encrypt_cbc(AES128Context *ctx, const uint8_t *plaintext, size_t len,
                       const uint8_t iv[16], uint8_t *ciphertext) {
    uint8_t prev[16];
    memcpy(prev, iv, 16);
    
    for (size_t i = 0; i < len; i += 16) {
        uint8_t block[16];
        
        // CBC模式：与前一个密文块异或
        for (int j = 0; j < 16; j++) {
            block[j] = plaintext[i + j] ^ prev[j];
        }
        
        // AES加密
        aes128_encrypt_ecb(ctx, block, block);
        
        // 保存结果
        memcpy(&ciphertext[i], block, 16);
        memcpy(prev, block, 16);
    }
}

// CBC模式解密
void aes128_decrypt_cbc(AES128Context *ctx, const uint8_t *ciphertext, size_t len,
                       const uint8_t iv[16], uint8_t *plaintext) {
    uint8_t prev[16];
    memcpy(prev, iv, 16);
    
    for (size_t i = 0; i < len; i += 16) {
        uint8_t block[16];
        memcpy(block, &ciphertext[i], 16);
        
        // 保存当前密文块
        uint8_t current[16];
        memcpy(current, block, 16);
        
        // AES解密
        aes128_decrypt_ecb(ctx, block, block);
        
        // CBC模式：与前一个密文块异或
        for (int j = 0; j < 16; j++) {
            plaintext[i + j] = block[j] ^ prev[j];
        }
        
        memcpy(prev, current, 16);
    }
}