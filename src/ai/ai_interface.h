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
//  ai_interface.h
//  AI专用二进制编辑器接口
//
//  设计原则：
//  1. 命令行参数驱动
//  2. JSON输出格式
//  3. 无交互模式
//  4. 状态码明确
//

#ifndef AI_INTERFACE_H
#define AI_INTERFACE_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "hex_editor.h"

// 操作模式
typedef enum {
    MODE_VIEW,          // 查看文件
    MODE_EDIT,          // 编辑字节
    MODE_INSERT,        // 插入字节
    MODE_DELETE,        // 删除字节
    MODE_SEARCH,        // 搜索字节
    MODE_EXPORT,        // 导出区域
    MODE_ANALYZE,       // 分析文件
    MODE_BATCH,         // 批处理
    MODE_AI,            // AI自然语言指令
    MODE_HELP           // 帮助
} OperationMode;

// 命令行参数
typedef struct {
    OperationMode mode;          // 操作模式
    char *filename;              // 文件名
    char *output_file;           // 输出文件
    char *batch_file;            // 批处理文件
    char *ai_command;            // AI指令
    
    // 编辑参数
    size_t offset;               // 偏移量
    uint8_t value;               // 字节值
    uint8_t *pattern;            // 搜索模式
    size_t pattern_len;          // 模式长度
    
    // 导出参数
    size_t start_offset;         // 起始偏移
    size_t end_offset;           // 结束偏移
    
    // 输出选项
    bool json_output;            // JSON输出
    bool quiet_mode;             // 安静模式
    bool force_overwrite;        // 强制覆盖
} CommandArgs;

// 函数声明

// 参数解析
CommandArgs* parse_args(int argc, const char *argv[]);
void free_args(CommandArgs *args);

// 操作执行
int execute_command(CommandArgs *args);
int execute_view(CommandArgs *args);
int execute_edit(CommandArgs *args);
int execute_insert(CommandArgs *args);
int execute_delete(CommandArgs *args);
int execute_search(CommandArgs *args);
int execute_export(CommandArgs *args);
int execute_analyze(CommandArgs *args);
int execute_batch(CommandArgs *args);
int execute_ai(CommandArgs *args);

// 工具函数
void print_json_header(const char *filename, size_t size);
void print_json_footer(void);
void print_json_section(size_t offset, const uint8_t *data, size_t size);
void print_json_edit_result(size_t offset, uint8_t old_value, uint8_t new_value);
void print_json_search_result(size_t offset, const uint8_t *pattern, size_t pattern_len);

// 错误处理
void print_error(const char *message);
void print_success(const char *message);

// 状态码
#define EXIT_SUCCESS 0
#define EXIT_INVALID_ARGS 1
#define EXIT_FILE_ERROR 2
#define EXIT_EDIT_ERROR 3
#define EXIT_MEMORY_ERROR 4
#define EXIT_AI_ERROR 5

#endif /* AI_INTERFACE_H */