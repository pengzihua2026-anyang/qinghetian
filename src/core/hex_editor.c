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
//  hex_editor.c
//  binary
//
//  Created by Zihua on 2026/4/1.
//

#include "hex_editor.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

// ==================== 文件操作 ====================

BinaryFile* binary_file_create(const char *filename) {
    BinaryFile *file = (BinaryFile*)malloc(sizeof(BinaryFile));
    if (!file) return NULL;
    
    file->data = NULL;
    file->size = 0;
    file->capacity = 0;
    file->modified = false;
    
    if (filename) {
        file->filename = strdup(filename);
    } else {
        file->filename = NULL;
    }
    
    return file;
}

bool binary_file_load(BinaryFile *file) {
    if (!file || !file->filename) return false;
    
    FILE *fp = fopen(file->filename, "rb");
    if (!fp) return false;
    
    // 获取文件大小
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    if (file_size < 0) {
        fclose(fp);
        return false;
    }
    
    // 分配内存
    size_t new_capacity = (size_t)file_size + 1024; // 额外空间
    uint8_t *new_data = (uint8_t*)malloc(new_capacity);
    if (!new_data) {
        fclose(fp);
        return false;
    }
    
    // 读取数据
    size_t bytes_read = fread(new_data, 1, file_size, fp);
    fclose(fp);
    
    if (bytes_read != (size_t)file_size) {
        free(new_data);
        return false;
    }
    
    // 更新文件结构
    free(file->data);
    file->data = new_data;
    file->size = bytes_read;
    file->capacity = new_capacity;
    file->modified = false;
    
    return true;
}

bool binary_file_save(BinaryFile *file) {
    if (!file || !file->filename || !file->data) return false;
    
    FILE *fp = fopen(file->filename, "wb");
    if (!fp) return false;
    
    size_t bytes_written = fwrite(file->data, 1, file->size, fp);
    fclose(fp);
    
    if (bytes_written == file->size) {
        file->modified = false;
        return true;
    }
    
    return false;
}

void binary_file_free(BinaryFile *file) {
    if (!file) return;
    
    free(file->data);
    free(file->filename);
    free(file);
}

bool binary_file_insert(BinaryFile *file, size_t offset, uint8_t byte) {
    if (!file || offset > file->size) return false;
    
    // 检查是否需要扩容
    if (file->size + 1 > file->capacity) {
        size_t new_capacity = file->capacity * 2 + 1024;
        uint8_t *new_data = (uint8_t*)realloc(file->data, new_capacity);
        if (!new_data) return false;
        
        file->data = new_data;
        file->capacity = new_capacity;
    }
    
    // 移动数据
    memmove(file->data + offset + 1, file->data + offset, file->size - offset);
    
    // 插入字节
    file->data[offset] = byte;
    file->size++;
    file->modified = true;
    
    return true;
}

bool binary_file_delete(BinaryFile *file, size_t offset) {
    if (!file || offset >= file->size) return false;
    
    // 移动数据
    memmove(file->data + offset, file->data + offset + 1, file->size - offset - 1);
    
    file->size--;
    file->modified = true;
    
    return true;
}

bool binary_file_replace(BinaryFile *file, size_t offset, uint8_t byte) {
    if (!file || offset >= file->size) return false;
    
    file->data[offset] = byte;
    file->modified = true;
    
    return true;
}

// ==================== 编辑器操作 ====================

EditorState* editor_create(void) {
    EditorState *editor = (EditorState*)malloc(sizeof(EditorState));
    if (!editor) return NULL;
    
    editor->file = NULL;
    editor->cursor_offset = 0;
    editor->view_offset = 0;
    editor->bytes_per_line = 16;
    editor->hex_mode = true;
    editor->running = true;
    
    return editor;
}

void editor_free(EditorState *editor) {
    if (!editor) return;
    
    if (editor->file) {
        binary_file_free(editor->file);
    }
    free(editor);
}

bool editor_open_file(EditorState *editor, const char *filename) {
    if (!editor) return false;
    
    // 如果已有打开的文件，先关闭
    if (editor->file) {
        binary_file_free(editor->file);
        editor->file = NULL;
    }
    
    BinaryFile *file = binary_file_create(filename);
    if (!file) return false;
    
    if (!binary_file_load(file)) {
        binary_file_free(file);
        return false;
    }
    
    editor->file = file;
    editor->cursor_offset = 0;
    editor->view_offset = 0;
    
    return true;
}

bool editor_save_file(EditorState *editor) {
    if (!editor || !editor->file) return false;
    
    return binary_file_save(editor->file);
}

// ==================== 显示函数 ====================

void clear_screen(void) {
    printf("\033[2J\033[H");
}

void print_hex_dump(const uint8_t *data, size_t size, size_t start_offset) {
    size_t i, j;
    
    for (i = 0; i < size; i += 16) {
        // 打印偏移量
        printf("%08zX: ", start_offset + i);
        
        // 打印十六进制
        for (j = 0; j < 16; j++) {
            if (i + j < size) {
                printf("%02X ", data[i + j]);
            } else {
                printf("   ");
            }
            
            if (j == 7) printf(" ");
        }
        
        printf(" ");
        
        // 打印ASCII
        for (j = 0; j < 16; j++) {
            if (i + j < size) {
                uint8_t c = data[i + j];
                printf("%c", isprint(c) ? c : '.');
            } else {
                printf(" ");
            }
        }
        
        printf("\n");
    }
}

void print_ascii_dump(const uint8_t *data, size_t size, size_t start_offset) {
    size_t i, j;
    
    for (i = 0; i < size; i += 16) {
        // 打印偏移量
        printf("%08zX: ", start_offset + i);
        
        // 打印ASCII
        for (j = 0; j < 16; j++) {
            if (i + j < size) {
                uint8_t c = data[i + j];
                printf("%c", isprint(c) ? c : '.');
            } else {
                printf(" ");
            }
            
            if (j == 7) printf(" ");
        }
        
        printf(" ");
        
        // 打印十六进制
        for (j = 0; j < 16; j++) {
            if (i + j < size) {
                printf("%02X ", data[i + j]);
            } else {
                printf("   ");
            }
        }
        
        printf("\n");
    }
}

void print_help(void) {
    printf("\n=== 二进制编辑器 - 帮助 ===\n");
    printf("导航:\n");
    printf("  ↑↓←→    - 移动光标\n");
    printf("  PageUp  - 上一页\n");
    printf("  PageDown- 下一页\n");
    printf("  Home    - 文件开头\n");
    printf("  End     - 文件结尾\n");
    printf("  G       - 跳转到偏移量\n");
    printf("\n编辑:\n");
    printf("  0-9,A-F - 输入十六进制值\n");
    printf("  Space   - 切换十六进制/ASCII模式\n");
    printf("  I       - 在光标处插入字节\n");
    printf("  D       - 删除光标处字节\n");
    printf("  R       - 替换光标处字节\n");
    printf("\n文件操作:\n");
    printf("  S       - 保存文件\n");
    printf("  Q       - 退出编辑器\n");
    printf("  H       - 显示帮助\n");
    printf("===========================\n");
}

void editor_display(EditorState *editor) {
    clear_screen();
    
    if (!editor->file) {
        printf("=== 二进制编辑器 ===\n");
        printf("没有打开的文件。使用: ./binary <文件名>\n");
        return;
    }
    
    // 打印文件信息
    printf("=== 二进制编辑器 ===\n");
    printf("文件: %s | 大小: %zu 字节 | %s\n", 
           editor->file->filename, 
           editor->file->size,
           editor->file->modified ? "[已修改]" : "[未修改]");
    printf("模式: %s | 光标: 0x%08zX\n", 
           editor->hex_mode ? "十六进制" : "ASCII", 
           editor->cursor_offset);
    printf("========================================\n");
    
    // 计算显示范围
    size_t display_size = editor->bytes_per_line * 20; // 显示20行
    if (editor->view_offset + display_size > editor->file->size) {
        display_size = editor->file->size - editor->view_offset;
    }
    
    // 显示数据
    if (editor->hex_mode) {
        print_hex_dump(editor->file->data + editor->view_offset, 
                      display_size, 
                      editor->view_offset);
    } else {
        print_ascii_dump(editor->file->data + editor->view_offset, 
                        display_size, 
                        editor->view_offset);
    }
    
    // 显示光标位置
    printf("\n");
    if (editor->cursor_offset >= editor->view_offset && 
        editor->cursor_offset < editor->view_offset + display_size) {
        size_t line = (editor->cursor_offset - editor->view_offset) / editor->bytes_per_line;
        size_t col = (editor->cursor_offset - editor->view_offset) % editor->bytes_per_line;
        
        printf("光标位置: 行 %zu, 列 %zu | ", line + 1, col + 1);
        printf("当前字节: 0x%02X", editor->file->data[editor->cursor_offset]);
        
        uint8_t c = editor->file->data[editor->cursor_offset];
        if (isprint(c)) {
            printf(" ('%c')", c);
        }
        printf("\n");
    }
    
    printf("\n按 H 显示帮助，Q 退出\n");
}

// ==================== 输入处理 ====================

int getch(void) {
    struct termios oldt, newt;
    int ch;
    
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    
    return ch;
}

void editor_process_input(EditorState *editor) {
    if (!editor) return;
    
    int ch = getch();
    
    switch (ch) {
        case 'q':
        case 'Q':
            editor->running = false;
            break;
            
        case 'h':
        case 'H':
            print_help();
            printf("\n按任意键继续...");
            getch();
            break;
            
        case 's':
        case 'S':
            if (editor->file) {
                if (editor_save_file(editor)) {
                    printf("文件已保存。\n");
                } else {
                    printf("保存失败。\n");
                }
                printf("按任意键继续...");
                getch();
            }
            break;
            
        case ' ':
            editor->hex_mode = !editor->hex_mode;
            break;
            
        case 'i':
        case 'I':
            if (editor->file) {
                // 插入字节
                if (binary_file_insert(editor->file, editor->cursor_offset, 0x00)) {
                    printf("已插入字节 0x00。\n");
                }
                printf("按任意键继续...");
                getch();
            }
            break;
            
        case 'd':
        case 'D':
            if (editor->file) {
                // 删除字节
                if (binary_file_delete(editor->file, editor->cursor_offset)) {
                    printf("已删除字节。\n");
                }
                printf("按任意键继续...");
                getch();
            }
            break;
            
        case 'g':
        case 'G':
            if (editor->file) {
                printf("跳转到偏移量 (十六进制): ");
                unsigned long offset;
                if (scanf("%lx", &offset) == 1) {
                    if (offset < editor->file->size) {
                        editor->cursor_offset = offset;
                        editor->view_offset = (offset / editor->bytes_per_line) * editor->bytes_per_line;
                    }
                }
                while (getchar() != '\n'); // 清空输入缓冲区
            }
            break;
            
        // 导航键
        case 27: // ESC 序列
            if (getch() == '[') {
                int ch2 = getch();
                switch (ch2) {
                    case 'A': // 上箭头
                        if (editor->cursor_offset >= editor->bytes_per_line) {
                            editor->cursor_offset -= editor->bytes_per_line;
                        }
                        break;
                    case 'B': // 下箭头
                        if (editor->cursor_offset + editor->bytes_per_line < editor->file->size) {
                            editor->cursor_offset += editor->bytes_per_line;
                        }
                        break;
                    case 'C': // 右箭头
                        if (editor->cursor_offset + 1 < editor->file->size) {
                            editor->cursor_offset++;
                        }
                        break;
                    case 'D': // 左箭头
                        if (editor->cursor_offset > 0) {
                            editor->cursor_offset--;
                        }
                        break;
                    case '5': // PageUp
                        if (getch() == '~') {
                            if (editor->view_offset >= editor->bytes_per_line * 10) {
                                editor->view_offset -= editor->bytes_per_line * 10;
                            }
                        }
                        break;
                    case '6': // PageDown
                        if (getch() == '~') {
                            if (editor->view_offset + editor->bytes_per_line * 10 < editor->file->size) {
                                editor->view_offset += editor->bytes_per_line * 10;
                            }
                        }
                        break;
                    case 'H': // Home
                        editor->cursor_offset = 0;
                        editor->view_offset = 0;
                        break;
                    case 'F': // End
                        if (editor->file) {
                            editor->cursor_offset = editor->file->size - 1;
                            editor->view_offset = (editor->cursor_offset / editor->bytes_per_line) * editor->bytes_per_line;
                        }
                        break;
                }
            }
            break;
            
        default:
            // 十六进制输入
            if (editor->hex_mode && editor->file) {
                if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F')) {
                    printf("输入第二个十六进制数字: ");
                    int ch2 = getch();
                    printf("%c\n", ch2);
                    
                    if ((ch2 >= '0' && ch2 <= '9') || (ch2 >= 'a' && ch2 <= 'f') || (ch2 >= 'A' && ch2 <= 'F')) {
                        // 转换十六进制
                        char hex[3] = { (char)ch, (char)ch2, '\0' };
                        uint8_t value = (uint8_t)strtol(hex, NULL, 16);
                        
                        if (binary_file_replace(editor->file, editor->cursor_offset, value)) {
                            printf("已修改为 0x%02X\n", value);
                            editor->cursor_offset++;
                            if (editor->cursor_offset >= editor->file->size) {
                                editor->cursor_offset = editor->file->size - 1;
                            }
                        }
                    }
                }
            } else if (!editor->hex_mode && editor->file) {
                // ASCII 输入
                if (isprint(ch)) {
                    if (binary_file_replace(editor->file, editor->cursor_offset, (uint8_t)ch)) {
                        printf("已修改为 '%c' (0x%02X)\n", ch, ch);
                        editor->cursor_offset++;
                        if (editor->cursor_offset >= editor->file->size) {
                            editor->cursor_offset = editor->file->size - 1;
                        }
                    }
                }
            }
            break;
    }
    
    // 确保光标在视图范围内
    if (editor->cursor_offset < editor->view_offset) {
        editor->view_offset = (editor->cursor_offset / editor->bytes_per_line) * editor->bytes_per_line;
    } else if (editor->cursor_offset >= editor->view_offset + editor->bytes_per_line * 20) {
        editor->view_offset = ((editor->cursor_offset - editor->bytes_per_line * 19) / editor->bytes_per_line) * editor->bytes_per_line;
    }
}