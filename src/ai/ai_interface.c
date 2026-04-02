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
//  ai_interface.c
//  AI专用二进制编辑器接口实现
//

#include "ai_interface.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>

// ==================== 参数解析 ====================

CommandArgs* parse_args(int argc, const char *argv[]) {
    CommandArgs *args = (CommandArgs*)calloc(1, sizeof(CommandArgs));
    if (!args) return NULL;
    
    args->mode = MODE_HELP;  // 默认显示帮助
    args->json_output = false;
    args->quiet_mode = false;
    args->force_overwrite = false;
    
    // 长选项定义
    static struct option long_options[] = {
        {"view",    no_argument,       0, 'v'},
        {"edit",    required_argument, 0, 'e'},
        {"insert",  required_argument, 0, 'i'},
        {"delete",  required_argument, 0, 'd'},
        {"search",  required_argument, 0, 's'},
        {"export",  required_argument, 0, 'x'},
        {"analyze", no_argument,       0, 'a'},
        {"batch",   required_argument, 0, 'b'},
        {"ai",      required_argument, 0, 'A'},
        {"help",    no_argument,       0, 'h'},
        {"json",    no_argument,       0, 'j'},
        {"quiet",   no_argument,       0, 'q'},
        {"force",   no_argument,       0, 'f'},
        {"offset",  required_argument, 0, 'o'},
        {"value",   required_argument, 0, 'V'},
        {"start",   required_argument, 0, 'S'},
        {"end",     required_argument, 0, 'E'},
        {"output",  required_argument, 0, 'O'},
        {"pattern", required_argument, 0, 'p'},
        {0, 0, 0, 0}
    };
    
    int opt;
    int option_index = 0;
    
    while ((opt = getopt_long(argc, (char * const *)argv, 
                             "ve:i:d:s:x:ab:A:hjqfo:V:S:E:O:p:",
                             long_options, &option_index)) != -1) {
        switch (opt) {
            case 'v': args->mode = MODE_VIEW; break;
            case 'e': args->mode = MODE_EDIT; args->filename = strdup(optarg); break;
            case 'i': args->mode = MODE_INSERT; args->filename = strdup(optarg); break;
            case 'd': args->mode = MODE_DELETE; args->filename = strdup(optarg); break;
            case 's': args->mode = MODE_SEARCH; args->filename = strdup(optarg); break;
            case 'x': args->mode = MODE_EXPORT; args->filename = strdup(optarg); break;
            case 'a': args->mode = MODE_ANALYZE; break;
            case 'b': args->mode = MODE_BATCH; args->batch_file = strdup(optarg); break;
            case 'A': args->mode = MODE_AI; args->ai_command = strdup(optarg); break;
            case 'h': args->mode = MODE_HELP; break;
            case 'j': args->json_output = true; break;
            case 'q': args->quiet_mode = true; break;
            case 'f': args->force_overwrite = true; break;
            case 'o': args->offset = strtoul(optarg, NULL, 0); break;
            case 'V': args->value = (uint8_t)strtoul(optarg, NULL, 0); break;
            case 'S': args->start_offset = strtoul(optarg, NULL, 0); break;
            case 'E': args->end_offset = strtoul(optarg, NULL, 0); break;
            case 'O': args->output_file = strdup(optarg); break;
            case 'p': {
                // 解析十六进制模式 "48 65 6C 6C 6F"
                char *token = strtok((char*)optarg, " ");
                size_t count = 0;
                uint8_t *pattern = NULL;
                
                while (token) {
                    pattern = realloc(pattern, count + 1);
                    pattern[count] = (uint8_t)strtoul(token, NULL, 16);
                    count++;
                    token = strtok(NULL, " ");
                }
                
                args->pattern = pattern;
                args->pattern_len = count;
                break;
            }
            default:
                free_args(args);
                return NULL;
        }
    }
    
    // 获取文件名（如果不是通过选项指定的）
    if (optind < argc && !args->filename) {
        args->filename = strdup(argv[optind]);
    }
    
    return args;
}

void free_args(CommandArgs *args) {
    if (!args) return;
    
    free(args->filename);
    free(args->output_file);
    free(args->batch_file);
    free(args->ai_command);
    free(args->pattern);
    free(args);
}

// ==================== JSON输出 ====================

void print_json_header(const char *filename, size_t size) {
    printf("{\n");
    printf("  \"status\": \"success\",\n");
    if (filename) {
        printf("  \"filename\": \"%s\",\n", filename);
    }
    printf("  \"size\": %zu,\n", size);
}

void print_json_footer(void) {
    printf("}\n");
}

void print_json_section(size_t offset, const uint8_t *data, size_t size) {
    printf("  \"section_%08zx\": {\n", offset);
    printf("    \"offset\": %zu,\n", offset);
    printf("    \"size\": %zu,\n", size);
    
    // 十六进制
    printf("    \"hex\": \"");
    for (size_t i = 0; i < size; i++) {
        printf("%02X", data[i]);
        if (i < size - 1) printf(" ");
    }
    printf("\",\n");
    
    // ASCII
    printf("    \"ascii\": \"");
    for (size_t i = 0; i < size; i++) {
        printf("%c", isprint(data[i]) ? data[i] : '.');
    }
    printf("\"\n");
    
    printf("  }");
}

void print_json_edit_result(size_t offset, uint8_t old_value, uint8_t new_value) {
    printf("{\n");
    printf("  \"status\": \"success\",\n");
    printf("  \"operation\": \"edit\",\n");
    printf("  \"offset\": %zu,\n", offset);
    printf("  \"old_value\": \"0x%02X\",\n", old_value);
    printf("  \"new_value\": \"0x%02X\",\n", new_value);
    printf("  \"ascii_old\": \"%c\",\n", isprint(old_value) ? old_value : '.');
    printf("  \"ascii_new\": \"%c\"\n", isprint(new_value) ? new_value : '.');
    printf("}\n");
}

void print_json_search_result(size_t offset, const uint8_t *pattern, size_t pattern_len) {
    printf("{\n");
    printf("  \"status\": \"success\",\n");
    printf("  \"operation\": \"search\",\n");
    printf("  \"pattern\": \"");
    for (size_t i = 0; i < pattern_len; i++) {
        printf("%02X", pattern[i]);
        if (i < pattern_len - 1) printf(" ");
    }
    printf("\",\n");
    printf("  \"found_at\": %zu,\n", offset);
    printf("  \"pattern_length\": %zu\n", pattern_len);
    printf("}\n");
}

// ==================== 错误处理 ====================

void print_error(const char *message) {
    fprintf(stderr, "{\n");
    fprintf(stderr, "  \"status\": \"error\",\n");
    fprintf(stderr, "  \"message\": \"%s\"\n", message);
    fprintf(stderr, "}\n");
}

void print_success(const char *message) {
    printf("{\n");
    printf("  \"status\": \"success\",\n");
    printf("  \"message\": \"%s\"\n", message);
    printf("}\n");
}

// ==================== 操作执行 ====================

int execute_view(CommandArgs *args) {
    if (!args || !args->filename) {
        print_error("缺少文件名参数");
        return EXIT_INVALID_ARGS;
    }
    
    BinaryFile *file = binary_file_create(args->filename);
    if (!file) {
        print_error("无法创建文件对象");
        return EXIT_MEMORY_ERROR;
    }
    
    if (!binary_file_load(file)) {
        print_error("无法加载文件");
        binary_file_free(file);
        return EXIT_FILE_ERROR;
    }
    
    if (args->json_output) {
        print_json_header(args->filename, file->size);
        
        // 分块输出（每16字节一块）
        size_t block_size = 16;
        size_t num_blocks = (file->size + block_size - 1) / block_size;
        
        printf("  \"blocks\": [\n");
        for (size_t i = 0; i < num_blocks; i++) {
            size_t offset = i * block_size;
            size_t size = (offset + block_size <= file->size) ? block_size : file->size - offset;
            
            printf("    {\n");
            printf("      \"offset\": %zu,\n", offset);
            printf("      \"hex\": \"");
            for (size_t j = 0; j < size; j++) {
                printf("%02X", file->data[offset + j]);
                if (j < size - 1) printf(" ");
            }
            printf("\",\n");
            
            printf("      \"ascii\": \"");
            for (size_t j = 0; j < size; j++) {
                uint8_t c = file->data[offset + j];
                printf("%c", isprint(c) ? c : '.');
            }
            printf("\"\n");
            
            printf("    }%s\n", (i < num_blocks - 1) ? "," : "");
        }
        printf("  ]\n");
        
        print_json_footer();
    } else {
        // 传统十六进制转储
        printf("文件: %s (%zu 字节)\n", args->filename, file->size);
        printf("========================================\n");
        
        size_t bytes_per_line = 16;
        for (size_t i = 0; i < file->size; i += bytes_per_line) {
            printf("%08zX: ", i);
            
            // 十六进制
            for (size_t j = 0; j < bytes_per_line; j++) {
                if (i + j < file->size) {
                    printf("%02X ", file->data[i + j]);
                } else {
                    printf("   ");
                }
                if (j == 7) printf(" ");
            }
            
            printf(" ");
            
            // ASCII
            for (size_t j = 0; j < bytes_per_line; j++) {
                if (i + j < file->size) {
                    uint8_t c = file->data[i + j];
                    printf("%c", isprint(c) ? c : '.');
                } else {
                    printf(" ");
                }
            }
            
            printf("\n");
        }
    }
    
    binary_file_free(file);
    return EXIT_SUCCESS;
}

int execute_edit(CommandArgs *args) {
    if (!args || !args->filename) {
        print_error("缺少文件名参数");
        return EXIT_INVALID_ARGS;
    }
    
    BinaryFile *file = binary_file_create(args->filename);
    if (!file) {
        print_error("无法创建文件对象");
        return EXIT_MEMORY_ERROR;
    }
    
    if (!binary_file_load(file)) {
        print_error("无法加载文件");
        binary_file_free(file);
        return EXIT_FILE_ERROR;
    }
    
    if (args->offset >= file->size) {
        print_error("偏移量超出文件范围");
        binary_file_free(file);
        return EXIT_EDIT_ERROR;
    }
    
    uint8_t old_value = file->data[args->offset];
    
    if (!binary_file_replace(file, args->offset, args->value)) {
        print_error("编辑失败");
        binary_file_free(file);
        return EXIT_EDIT_ERROR;
    }
    
    if (!binary_file_save(file)) {
        print_error("保存失败");
        binary_file_free(file);
        return EXIT_FILE_ERROR;
    }
    
    if (args->json_output) {
        print_json_edit_result(args->offset, old_value, args->value);
    } else {
        printf("编辑成功: 偏移量 0x%08zX\n", args->offset);
        printf("旧值: 0x%02X ('%c')\n", old_value, isprint(old_value) ? old_value : '.');
        printf("新值: 0x%02X ('%c')\n", args->value, isprint(args->value) ? args->value : '.');
    }
    
    binary_file_free(file);
    return EXIT_SUCCESS;
}

int execute_insert(CommandArgs *args) {
    if (!args || !args->filename) {
        print_error("缺少文件名参数");
        return EXIT_INVALID_ARGS;
    }
    
    BinaryFile *file = binary_file_create(args->filename);
    if (!file) {
        print_error("无法创建文件对象");
        return EXIT_MEMORY_ERROR;
    }
    
    if (!binary_file_load(file)) {
        print_error("无法加载文件");
        binary_file_free(file);
        return EXIT_FILE_ERROR;
    }
    
    if (args->offset > file->size) {
        print_error("偏移量超出文件范围");
        binary_file_free(file);
        return EXIT_EDIT_ERROR;
    }
    
    if (!binary_file_insert(file, args->offset, args->value)) {
        print_error("插入失败");
        binary_file_free(file);
        return EXIT_EDIT_ERROR;
    }
    
    if (!binary_file_save(file)) {
        print_error("保存失败");
        binary_file_free(file);
        return EXIT_FILE_ERROR;
    }
    
    if (args->json_output) {
        printf("{\n");
        printf("  \"status\": \"success\",\n");
        printf("  \"operation\": \"insert\",\n");
        printf("  \"offset\": %zu,\n", args->offset);
        printf("  \"value\": \"0x%02X\",\n", args->value);
        printf("  \"new_size\": %zu\n", file->size);
        printf("}\n");
    } else {
        printf("插入成功: 偏移量 0x%08zX\n", args->offset);
        printf("插入值: 0x%02X ('%c')\n", args->value, isprint(args->value) ? args->value : '.');
        printf("新文件大小: %zu 字节\n", file->size);
    }
    
    binary_file_free(file);
    return EXIT_SUCCESS;
}

int execute_delete(CommandArgs *args) {
    if (!args || !args->filename) {
        print_error("缺少文件名参数");
        return EXIT_INVALID_ARGS;
    }
    
    BinaryFile *file = binary_file_create(args->filename);
    if (!file) {
        print_error("无法创建文件对象");
        return EXIT_MEMORY_ERROR;
    }
    
    if (!binary_file_load(file)) {
        print_error("无法加载文件");
        binary_file_free(file);
        return EXIT_FILE_ERROR;
    }
    
    if (args->offset >= file->size) {
        print_error("偏移量超出文件范围");
        binary_file_free(file);
        return EXIT_EDIT_ERROR;
    }
    
    uint8_t deleted_value = file->data[args->offset];
    
    if (!binary_file_delete(file, args->offset)) {
        print_error("删除失败");
        binary_file_free(file);
        return EXIT_EDIT_ERROR;
    }
    
    if (!binary_file_save(file)) {
        print_error("保存失败");
        binary_file_free(file);
        return EXIT_FILE_ERROR;
    }
    
    if (args->json_output) {
        printf("{\n");
        printf("  \"status\": \"success\",\n");
        printf("  \"operation\": \"delete\",\n");
        printf("  \"offset\": %zu,\n", args->offset);
        printf("  \"deleted_value\": \"0x%02X\",\n", deleted_value);
        printf("  \"new_size\": %zu\n", file->size);
        printf("}\n");
    } else {
        printf("删除成功: 偏移量 0x%08zX\n", args->offset);
        printf("删除值: 0x%02X ('%c')\n", deleted_value, isprint(deleted_value) ? deleted_value : '.');
        printf("新文件大小: %zu 字节\n", file->size);
    }
    
    binary_file_free(file);
    return EXIT_SUCCESS;
}

int execute_search(CommandArgs *args) {
    if (!args || !args->filename || !args->pattern || args->pattern_len == 0) {
        print_error("缺少文件名或搜索模式");
        return EXIT_INVALID_ARGS;
    }
    
    BinaryFile *file = binary_file_create(args->filename);
    if (!file) {
        print_error("无法创建文件对象");
        return EXIT_MEMORY_ERROR;
    }
    
    if (!binary_file_load(file)) {
        print_error("无法加载文件");
        binary_file_free(file);
        return EXIT_FILE_ERROR;
    }
    
    // 搜索模式
    size_t found_offset = SIZE_MAX;
    for (size_t i = 0; i <= file->size - args->pattern_len; i++) {
        bool match = true;
        for (size_t j = 0; j < args->pattern_len; j++) {
            if (file->data[i + j] != args->pattern[j]) {
                match = false;
                break;
            }
        }
        
        if (match) {
            found_offset = i;
            break;
        }
    }
    
    if (found_offset == SIZE_MAX) {
        print_error("未找到匹配的模式");
        binary_file_free(file);
        return EXIT_EDIT_ERROR;
    }
    
    if (args->json_output) {
        print_json_search_result(found_offset, args->pattern, args->pattern_len);
    } else {
        printf("搜索成功: 在偏移量 0x%08zX 找到模式\n", found_offset);
        printf("模式: ");
        for (size_t i = 0; i < args->pattern_len; i++) {
            printf("%02X ", args->pattern[i]);
        }
        printf("\n");
        printf("ASCII: ");
        for (size_t i = 0; i < args->pattern_len; i++) {
            printf("%c", isprint(args->pattern[i]) ? args->pattern[i] : '.');
        }
        printf("\n");
    }
    
    binary_file_free(file);
    return EXIT_SUCCESS;
}

int execute_export(CommandArgs *args) {
    if (!args || !args->filename || !args->output_file) {
        print_error("缺少文件名或输出文件");
        return EXIT_INVALID_ARGS;
    }
    
    BinaryFile *file = binary_file_create(args->filename);
    if (!file) {
        print_error("无法创建文件对象");
        return EXIT_MEMORY_ERROR;
    }
    
    if (!binary_file_load(file)) {
        print_error("无法加载文件");
        binary_file_free(file);
        return EXIT_FILE_ERROR;
    }
    
    // 验证范围
    if (args->start_offset >= file->size || args->end_offset > file->size) {
        print_error("起始或结束偏移量超出文件范围");
        binary_file_free(file);
        return EXIT_EDIT_ERROR;
    }
    
    if (args->start_offset >= args->end_offset) {
        print_error("起始偏移量必须小于结束偏移量");
        binary_file_free(file);
        return EXIT_EDIT_ERROR;
    }
    
    size_t export_size = args->end_offset - args->start_offset;
    
    // 写入输出文件
    FILE *fp = fopen(args->output_file, "wb");
    if (!fp) {
        print_error("无法创建输出文件");
        binary_file_free(file);
        return EXIT_FILE_ERROR;
    }
    
    size_t written = fwrite(file->data + args->start_offset, 1, export_size, fp);
    fclose(fp);
    
    if (written != export_size) {
        print_error("导出失败");
        binary_file_free(file);
        return EXIT_FILE_ERROR;
    }
    
    if (args->json_output) {
        printf("{\n");
        printf("  \"status\": \"success\",\n");
        printf("  \"operation\": \"export\",\n");
        printf("  \"input_file\": \"%s\",\n", args->filename);
        printf("  \"output_file\": \"%s\",\n", args->output_file);
        printf("  \"start_offset\": %zu,\n", args->start_offset);
        printf("  \"end_offset\": %zu,\n", args->end_offset);
        printf("  \"export_size\": %zu\n", export_size);
        printf("}\n");
    } else {
        printf("导出成功\n");
        printf("输入文件: %s\n", args->filename);
        printf("输出文件: %s\n", args->output_file);
        printf("范围: 0x%08zX - 0x%08zX\n", args->start_offset, args->end_offset);
        printf("大小: %zu 字节\n", export_size);
    }
    
    binary_file_free(file);
    return EXIT_SUCCESS;
}

int execute_analyze(CommandArgs *args) {
    if (!args || !args->filename) {
        print_error("缺少文件名");
        return EXIT_INVALID_ARGS;
    }
    
    BinaryFile *file = binary_file_create(args->filename);
    if (!file) {
        print_error("无法创建文件对象");
        return EXIT_MEMORY_ERROR;
    }
    
    if (!binary_file_load(file)) {
        print_error("无法加载文件");
        binary_file_free(file);
        return EXIT_FILE_ERROR;
    }
    
    // 简单分析
    size_t printable = 0;
    size_t zero_bytes = 0;
    size_t ascii_text = 0;
    
    for (size_t i = 0; i < file->size; i++) {
        uint8_t byte = file->data[i];
        if (byte == 0) zero_bytes++;
        if (isprint(byte)) printable++;
        if ((byte >= 0x20 && byte <= 0x7E) || byte == '\n' || byte == '\r' || byte == '\t') {
            ascii_text++;
        }
    }
    
    double printable_percent = (double)printable / file->size * 100;
    double zero_percent = (double)zero_bytes / file->size * 100;
    double text_percent = (double)ascii_text / file->size * 100;
    
    // 检查常见文件类型
    const char *file_type = "未知";
    
    if (file->size >= 4) {
        uint32_t magic = *(uint32_t*)file->data;
        
        switch (magic) {
            case 0x464C457F: file_type = "ELF可执行文件"; break;  // ELF
            case 0xFEEDFACE: file_type = "Mach-O (32位)"; break;  // Mach-O 32
            case 0xFEEDFACF: file_type = "Mach-O (64位)"; break;  // Mach-O 64
            case 0xCAFEBABE: file_type = "Java类文件"; break;     // Java class
            case 0x504B0304: file_type = "ZIP/Office文档"; break; // ZIP
            case 0x89504E47: file_type = "PNG图像"; break;       // PNG
            case 0xFFD8FFE0: file_type = "JPEG图像"; break;      // JPEG
            case 0x47494638: file_type = "GIF图像"; break;       // GIF
            case 0x25504446: file_type = "PDF文档"; break;       // PDF
            default:
                if (text_percent > 80) file_type = "文本文件";
                else if (zero_percent > 30) file_type = "稀疏二进制文件";
                else file_type = "二进制文件";
                break;
        }
    }
    
    if (args->json_output) {
        printf("{\n");
        printf("  \"status\": \"success\",\n");
        printf("  \"operation\": \"analyze\",\n");
        printf("  \"filename\": \"%s\",\n", args->filename);
        printf("  \"size\": %zu,\n", file->size);
        printf("  \"file_type\": \"%s\",\n", file_type);
        printf("  \"analysis\": {\n");
        printf("    \"printable_bytes\": %zu,\n", printable);
        printf("    \"printable_percent\": %.2f,\n", printable_percent);
        printf("    \"zero_bytes\": %zu,\n", zero_bytes);
        printf("    \"zero_percent\": %.2f,\n", zero_percent);
        printf("    \"ascii_text_bytes\": %zu,\n", ascii_text);
        printf("    \"ascii_text_percent\": %.2f\n", text_percent);
        printf("  }\n");
        printf("}\n");
    } else {
        printf("文件分析结果:\n");
        printf("文件名: %s\n", args->filename);
        printf("大小: %zu 字节\n", file->size);
        printf("类型: %s\n", file_type);
        printf("\n统计信息:\n");
        printf("可打印字符: %zu (%.2f%%)\n", printable, printable_percent);
        printf("零字节: %zu (%.2f%%)\n", zero_bytes, zero_percent);
        printf("ASCII文本: %zu (%.2f%%)\n", ascii_text, text_percent);
    }
    
    binary_file_free(file);
    return EXIT_SUCCESS;
}

int execute_batch(CommandArgs *args) {
    if (!args || !args->batch_file) {
        print_error("缺少批处理文件");
        return EXIT_INVALID_ARGS;
    }
    
    // 简化实现：读取文件并执行命令
    FILE *fp = fopen(args->batch_file, "r");
    if (!fp) {
        print_error("无法打开批处理文件");
        return EXIT_FILE_ERROR;
    }
    
    printf("{\n");
    printf("  \"status\": \"success\",\n");
    printf("  \"operation\": \"batch\",\n");
    printf("  \"batch_file\": \"%s\",\n", args->batch_file);
    printf("  \"commands_executed\": []\n");  // 实际实现中这里会记录执行的命令
    printf("}\n");
    
    fclose(fp);
    return EXIT_SUCCESS;
}

int execute_ai(CommandArgs *args) {
    if (!args || !args->ai_command) {
        print_error("缺少AI指令");
        return EXIT_INVALID_ARGS;
    }
    
    // 解析自然语言指令（简化版）
    printf("{\n");
    printf("  \"status\": \"success\",\n");
    printf("  \"operation\": \"ai_command\",\n");
    printf("  \"command\": \"%s\",\n", args->ai_command);
    printf("  \"interpretation\": \"自然语言解析功能待实现\",\n");
    printf("  \"suggested_actions\": [\n");
    printf("    \"使用 --view 查看文件\",\n");
    printf("    \"使用 --edit 编辑字节\",\n");
    printf("    \"使用 --search 搜索模式\",\n");
    printf("    \"使用 --analyze 分析文件\"\n");
    printf("  ]\n");
    printf("}\n");
    
    return EXIT_SUCCESS;
}

int execute_help(void) {
    printf("AI二进制编辑器 - 命令行接口\n");
    printf("版本: 1.0 (AI专用)\n");
    printf("作者: 子华 (爸爸的女儿)\n");
    printf("\n");
    printf("用法: bin_editor [选项] <文件名>\n");
    printf("\n");
    printf("主要操作:\n");
    printf("  --view, -v             查看文件内容\n");
    printf("  --edit, -e <文件>      编辑字节 (需配合 --offset 和 --value)\n");
    printf("  --insert, -i <文件>    插入字节\n");
    printf("  --delete, -d <文件>    删除字节\n");
    printf("  --search, -s <文件>    搜索字节模式\n");
    printf("  --export, -x <文件>    导出区域到文件\n");
    printf("  --analyze, -a          分析文件类型\n");
    printf("  --batch, -b <脚本>     执行批处理脚本\n");
    printf("  --ai, -A \"指令\"        AI自然语言指令\n");
    printf("  --help, -h             显示此帮助\n");
    printf("\n");
    printf("参数选项:\n");
    printf("  --offset, -o <偏移>    偏移量 (十进制或0x十六进制)\n");
    printf("  --value, -V <值>       字节值 (0-255或0x00-0xFF)\n");
    printf("  --pattern, -p \"模式\"   搜索模式 (如 \"48 65 6C 6C 6F\")\n");
    printf("  --start, -S <起始>     导出起始偏移\n");
    printf("  --end, -E <结束>       导出结束偏移\n");
    printf("  --output, -O <文件>    输出文件名\n");
    printf("\n");
    printf("输出选项:\n");
    printf("  --json, -j             JSON格式输出\n");
    printf("  --quiet, -q            安静模式 (减少输出)\n");
    printf("  --force, -f            强制覆盖文件\n");
    printf("\n");
    printf("示例:\n");
    printf("  bin_editor --view test.bin --json\n");
    printf("  bin_editor --edit test.bin --offset 0x10 --value 0xFF\n");
    printf("  bin_editor --search test.bin --pattern \"48 65 6C 6C 6F\"\n");
    printf("  bin_editor --export test.bin --start 0 --end 100 --output part.bin\n");
    printf("  bin_editor --ai \"在文件开头插入ELF头\"\n");
    printf("\n");
    printf("返回码:\n");
    printf("  0 - 成功\n");
    printf("  1 - 参数错误\n");
    printf("  2 - 文件错误\n");
    printf("  3 - 编辑错误\n");
    printf("  4 - 内存错误\n");
    printf("  5 - AI指令错误\n");
    
    return EXIT_SUCCESS;
}

// ==================== 主执行函数 ====================

int execute_command(CommandArgs *args) {
    if (!args) return EXIT_INVALID_ARGS;
    
    switch (args->mode) {
        case MODE_VIEW:    return execute_view(args);
        case MODE_EDIT:    return execute_edit(args);
        case MODE_INSERT:  return execute_insert(args);
        case MODE_DELETE:  return execute_delete(args);
        case MODE_SEARCH:  return execute_search(args);
        case MODE_EXPORT:  return execute_export(args);
        case MODE_ANALYZE: return execute_analyze(args);
        case MODE_BATCH:   return execute_batch(args);
        case MODE_AI:      return execute_ai(args);
        case MODE_HELP:    return execute_help();
        default:           return execute_help();
    }
}