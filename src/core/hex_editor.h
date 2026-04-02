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
//  hex_editor.h
//  binary
//
//  Created by Zihua on 2026/4/1.
//

#ifndef hex_editor_h
#define hex_editor_h

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// 文件操作相关
typedef struct {
    uint8_t *data;          // 文件数据
    size_t size;            // 文件大小
    size_t capacity;        // 缓冲区容量
    char *filename;         // 文件名
    bool modified;          // 是否已修改
} BinaryFile;

// 编辑器状态
typedef struct {
    BinaryFile *file;       // 当前文件
    size_t cursor_offset;   // 光标偏移量
    size_t view_offset;     // 视图偏移量
    size_t bytes_per_line;  // 每行显示的字节数
    bool hex_mode;          // 十六进制模式 (true) 或 ASCII 模式 (false)
    bool running;           // 是否运行中
} EditorState;

// 函数声明

// 文件操作
BinaryFile* binary_file_create(const char *filename);
bool binary_file_load(BinaryFile *file);
bool binary_file_save(BinaryFile *file);
void binary_file_free(BinaryFile *file);
bool binary_file_insert(BinaryFile *file, size_t offset, uint8_t byte);
bool binary_file_delete(BinaryFile *file, size_t offset);
bool binary_file_replace(BinaryFile *file, size_t offset, uint8_t byte);

// 编辑器操作
EditorState* editor_create(void);
void editor_free(EditorState *editor);
bool editor_open_file(EditorState *editor, const char *filename);
bool editor_save_file(EditorState *editor);
void editor_display(EditorState *editor);
void editor_process_input(EditorState *editor);

// 工具函数
void print_hex_dump(const uint8_t *data, size_t size, size_t start_offset);
void print_ascii_dump(const uint8_t *data, size_t size, size_t start_offset);
void clear_screen(void);
void print_help(void);

#endif /* hex_editor_h */