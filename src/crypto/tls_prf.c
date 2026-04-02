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
//  tls_prf.c
//  TLS PRF实现（简化版）
//

#include "mini_tls.h"
#include <string.h>

// 简化版TLS PRF（实际应该使用HMAC-SHA256）
void tls_prf(const uint8_t *secret, size_t secret_len,
            const char *label, size_t label_len,
            const uint8_t *seed, size_t seed_len,
            uint8_t *output, size_t output_len) {
    // 简化实现：使用SHA256(secret + label + seed)
    // 实际TLS PRF更复杂，但这里先简化
    
    uint8_t buffer[256];
    size_t pos = 0;
    
    // 构建输入：secret + label + seed
    if (secret_len > 0 && secret) {
        memcpy(buffer + pos, secret, secret_len);
        pos += secret_len;
    }
    
    if (label_len > 0 && label) {
        memcpy(buffer + pos, label, label_len);
        pos += label_len;
    }
    
    if (seed_len > 0 && seed) {
        memcpy(buffer + pos, seed, seed_len);
        pos += seed_len;
    }
    
    // 计算SHA256
    uint8_t hash[32];
    sha256(buffer, pos, hash);
    
    // 复制到输出（如果输出长度超过32字节，重复哈希）
    size_t copied = 0;
    while (copied < output_len) {
        size_t to_copy = 32;
        if (output_len - copied < 32) {
            to_copy = output_len - copied;
        }
        
        memcpy(output + copied, hash, to_copy);
        copied += to_copy;
        
        // 如果需要更多数据，再次哈希
        if (copied < output_len) {
            // 哈希当前哈希值
            sha256(hash, 32, hash);
        }
    }
}