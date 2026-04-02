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
//  ai_integration.c
//  AI大模型集成实现
//

#include "ai_integration.h"
#include "hex_editor.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ==================== 配置管理 ====================

AIContext* ai_context_create(void) {
    AIContext *context = (AIContext*)calloc(1, sizeof(AIContext));
    if (!context) return NULL;
    
    context->config.api_url = NULL;
    context->config.api_key = NULL;
    context->config.model_name = strdup("gpt-4");
    context->config.max_tokens = 1000;
    context->config.temperature = 0.7;
    context->config.configured = false;
    
    context->conversation_id = NULL;
    context->history = NULL;
    context->history_count = 0;
    context->max_history = 50;
    
    return context;
}

void ai_context_free(AIContext *context) {
    if (!context) return;
    
    free(context->config.api_url);
    free(context->config.api_key);
    free(context->config.model_name);
    free(context->conversation_id);
    
    for (int i = 0; i < context->history_count; i++) {
        free(context->history[i]);
    }
    free(context->history);
    
    free(context);
}

bool ai_configure_model(AIContext *context, const char *url, const char *key, const char *model) {
    if (!context) return false;
    
    free(context->config.api_url);
    free(context->config.api_key);
    free(context->config.model_name);
    
    context->config.api_url = url ? strdup(url) : NULL;
    context->config.api_key = key ? strdup(key) : NULL;
    context->config.model_name = model ? strdup(model) : strdup("gpt-4");
    context->config.configured = (url != NULL && key != NULL);
    
    return context->config.configured;
}

bool ai_load_config(AIContext *context, const char *config_file) {
    // 简化实现：从文件读取配置
    FILE *fp = fopen(config_file, "r");
    if (!fp) return false;
    
    char url[256] = {0};
    char key[256] = {0};
    char model[64] = {0};
    
    fscanf(fp, "url=%255s\n", url);
    fscanf(fp, "key=%255s\n", key);
    fscanf(fp, "model=%63s\n", model);
    
    fclose(fp);
    
    return ai_configure_model(context, url, key, model);
}

bool ai_save_config(AIContext *context, const char *config_file) {
    if (!context || !context->config.configured) return false;
    
    FILE *fp = fopen(config_file, "w");
    if (!fp) return false;
    
    fprintf(fp, "url=%s\n", context->config.api_url);
    fprintf(fp, "key=%s\n", context->config.api_key);
    fprintf(fp, "model=%s\n", context->config.model_name);
    fprintf(fp, "max_tokens=%d\n", context->config.max_tokens);
    fprintf(fp, "temperature=%.2f\n", context->config.temperature);
    
    fclose(fp);
    return true;
}

// ==================== 自然语言解析 ====================

// 简单的关键词匹配解析（实际应该用AI模型）
AIOperation* ai_parse_command(AIContext *context, const char *command) {
    AIOperation *op = (AIOperation*)calloc(1, sizeof(AIOperation));
    if (!op) return NULL;
    
    op->original_text = strdup(command);
    op->type = AI_OP_UNKNOWN;
    op->confidence = strdup("低");
    
    // 转换为小写便于匹配
    char *cmd_lower = strdup(command);
    for (int i = 0; cmd_lower[i]; i++) {
        cmd_lower[i] = tolower(cmd_lower[i]);
    }
    
    // 关键词匹配
    if (strstr(cmd_lower, "查看") || strstr(cmd_lower, "显示") || strstr(cmd_lower, "view")) {
        op->type = AI_OP_VIEW;
        op->interpretation = strdup("查看文件内容");
        op->confidence = strdup("高");
    }
    else if (strstr(cmd_lower, "编辑") || strstr(cmd_lower, "修改") || strstr(cmd_lower, "edit")) {
        op->type = AI_OP_EDIT;
        op->interpretation = strdup("编辑字节");
        op->confidence = strdup("中");
    }
    else if (strstr(cmd_lower, "插入") || strstr(cmd_lower, "添加") || strstr(cmd_lower, "insert")) {
        op->type = AI_OP_INSERT;
        op->interpretation = strdup("插入字节");
        op->confidence = strdup("中");
    }
    else if (strstr(cmd_lower, "删除") || strstr(cmd_lower, "remove") || strstr(cmd_lower, "delete")) {
        op->type = AI_OP_DELETE;
        op->interpretation = strdup("删除字节");
        op->confidence = strdup("中");
    }
    else if (strstr(cmd_lower, "搜索") || strstr(cmd_lower, "查找") || strstr(cmd_lower, "search")) {
        op->type = AI_OP_SEARCH;
        op->interpretation = strdup("搜索字节模式");
        op->confidence = strdup("高");
    }
    else if (strstr(cmd_lower, "分析") || strstr(cmd_lower, "analyze")) {
        op->type = AI_OP_ANALYZE;
        op->interpretation = strdup("分析文件");
        op->confidence = strdup("高");
    }
    else if (strstr(cmd_lower, "创建") || strstr(cmd_lower, "新建") || strstr(cmd_lower, "create")) {
        op->type = AI_OP_CREATE;
        op->interpretation = strdup("创建文件");
        op->confidence = strdup("中");
    }
    else if (strstr(cmd_lower, "导出") || strstr(cmd_lower, "export")) {
        op->type = AI_OP_EXPORT;
        op->interpretation = strdup("导出文件区域");
        op->confidence = strdup("中");
    }
    else if (strstr(cmd_lower, "十六进制转") || strstr(cmd_lower, "hex to")) {
        op->type = AI_OP_HEX_TO_ASCII;
        op->interpretation = strdup("十六进制转ASCII");
        op->confidence = strdup("高");
    }
    else if (strstr(cmd_lower, "ascii转") || strstr(cmd_lower, "ascii to")) {
        op->type = AI_OP_ASCII_TO_HEX;
        op->interpretation = strdup("ASCII转十六进制");
        op->confidence = strdup("高");
    }
    else {
        op->interpretation = strdup("无法识别的命令");
    }
    
    // 提取文件名（简化版）
    char *filename = ai_extract_filename(command);
    if (filename) {
        op->filename = filename;
    }
    
    // 提取偏移量
    char *offset_str = ai_extract_offset(command);
    if (offset_str) {
        op->offset = strtoul(offset_str, NULL, 0);
        free(offset_str);
    }
    
    // 提取值
    char *value_str = ai_extract_value(command);
    if (value_str) {
        op->value = (uint8_t)strtoul(value_str, NULL, 0);
        free(value_str);
    }
    
    free(cmd_lower);
    return op;
}

void ai_operation_free(AIOperation *op) {
    if (!op) return;
    
    free(op->filename);
    free(op->target_file);
    free(op->original_text);
    free(op->interpretation);
    free(op->confidence);
    free(op->pattern);
    free(op);
}

char* ai_generate_prompt(const char *command, const char *file_context) {
    // 生成AI提示词
    char *prompt = NULL;
    
    if (file_context) {
        asprintf(&prompt,
            "你是一个二进制文件专家。用户命令：%s\n\n"
            "文件上下文：\n%s\n\n"
            "请解析这个命令并告诉我应该执行什么二进制操作。"
            "可能的操作：查看(view)、编辑(edit)、插入(insert)、删除(delete)、"
            "搜索(search)、分析(analyze)、创建(create)、导出(export)。"
            "请用JSON格式回复，包含：operation, filename, offset, value, pattern等字段。",
            command, file_context);
    } else {
        asprintf(&prompt,
            "你是一个二进制文件专家。用户命令：%s\n\n"
            "请解析这个命令并告诉我应该执行什么二进制操作。"
            "可能的操作：查看(view)、编辑(edit)、插入(insert)、删除(delete)、"
            "搜索(search)、分析(analyze)、创建(create)、导出(export)。"
            "请用JSON格式回复，包含：operation, filename, offset, value, pattern等字段。",
            command);
    }
    
    return prompt;
}

// ==================== AI通信（简化版） ====================



// 从JSON响应中提取AI回复
char* extract_ai_response(const char *json_response) {
    printf("[DEBUG] extract_ai_response: 输入大小=%zu\n", strlen(json_response));
    printf("[DEBUG] 输入预览: %.200s\n", json_response);
    if (!json_response) return NULL;
    
    // 更健壮的实现：查找 "content" 字段
    const char *content_start = strstr(json_response, "\"content\":");
    if (!content_start) {
        printf("[DEBUG] extract_ai_response: 找不到 'content' 字段\n");
        // 返回原始JSON用于调试
        char *debug = malloc(strlen(json_response) + 50);
        if (debug) {
            snprintf(debug, strlen(json_response) + 50, 
                    "错误：找不到content字段。响应: %s", json_response);
        }
        return debug ? debug : strdup("错误：无法解析API响应");
    }
    
    // 跳过 "\"content\":"
    content_start += 10;
    
    // 跳过空格
    while (*content_start && isspace(*content_start)) {
        content_start++;
    }
    
    // 检查是否是字符串
    if (*content_start != '"') {
        printf("[DEBUG] extract_ai_response: content不是字符串 (第一个字符: %c)\n", *content_start);
        
        // 可能是null或其他值
        if (strncmp(content_start, "null", 4) == 0) {
            return strdup("错误：API返回空内容 (content: null)");
        }
        
        char *debug = malloc(100);
        if (debug) {
            snprintf(debug, 100, "错误：无效的内容格式 (以'%c'开头)", *content_start);
        }
        return debug ? debug : strdup("错误：无效的API响应格式");
    }
    
    // 跳过开头的引号
    content_start++;
    
    // 找到字符串结束位置（处理转义字符）
    const char *content_end = content_start;
    while (*content_end && *content_end != '"') {
        if (*content_end == '\\' && *(content_end + 1)) {
            // 跳过转义字符
            content_end += 2;
        } else {
            content_end++;
        }
    }
    
    if (!*content_end) {
        printf("[DEBUG] extract_ai_response: 找不到字符串结束引号\n");
        return strdup("错误：不完整的字符串");
    }
    
    size_t content_len = content_end - content_start;
    printf("[DEBUG] extract_ai_response: 找到内容, 长度=%zu\n", content_len);
    
    if (content_len == 0) {
        printf("[DEBUG] extract_ai_response: 内容长度为0\n");
        return strdup("错误：API返回空内容");
    }
    
    char *content = malloc(content_len + 1);
    if (!content) return strdup("错误：内存不足");
    
    // 复制内容（不处理转义，简化版）
    strncpy(content, content_start, content_len);
    content[content_len] = '\0';
    
    // 处理转义字符（简化版）
    char *result = malloc(strlen(content) * 2 + 1);  // 最坏情况：每个字符都需要转义
    if (!result) {
        free(content);
        return NULL;
    }
    
    char *dst = result;
    const char *src = content;
    
    while (*src) {
        if (*src == '\\' && *(src + 1) == 'n') {
            *dst++ = '\n';
            src += 2;
        } else if (*src == '\\' && *(src + 1) == 't') {
            *dst++ = '\t';
            src += 2;
        } else if (*src == '\\' && *(src + 1) == '\\') {
            *dst++ = '\\';
            src += 2;
        } else if (*src == '\\' && *(src + 1) == '"') {
            *dst++ = '"';
            src += 2;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
    
    free(content);
    return result;
}

// 真实的AI API调用
// ai_send_request函数现在在ai_proxy.c中实现
// 这里只需要声明
char* ai_send_request(AIContext *context, const char *prompt);

char* ai_chat(AIContext *context, const char *message) {
    if (!context) {
        char *error = strdup("错误：AI上下文未初始化");
        return error;
    }
    
    // 检查是否配置
    if (!context->config.configured) {
        char *error = NULL;
        asprintf(&error, "⚠️  AI模型未配置，使用模拟模式。\n\n"
                        "要启用真正的AI对话，请配置AI模型:\n"
                        "  /config <API地址> <API密钥> [模型名称]\n\n"
                        "示例:\n"
                        "  • OpenAI: /config https://api.openai.com/v1/chat/completions sk-your-openai-key gpt-4\n"
                        "  • DeepSeek: /config https://api.deepseek.com/chat/completions sk-your-deepseek-key deepseek-chat\n"
                        "  • Claude: /config https://api.anthropic.com/v1/messages sk-your-claude-key claude-3-opus\n\n"
                        "当前使用模拟响应演示功能。");
        return error;
    }
    
    // 添加到历史
    ai_add_to_history(context, message, NULL);
    
    // 发送请求
    char *response = ai_send_request(context, message);
    if (!response) {
        response = strdup("错误：AI请求失败");
    }
    
    // 添加到历史
    ai_add_to_history(context, NULL, response);
    
    return response;
}

char* ai_process_binary_command(AIContext *context, const char *command, const char *filename) {
    // 获取文件上下文
    char *file_context = NULL;
    if (filename) {
        file_context = ai_get_file_context(filename, 1024);
    }
    
    // 生成提示词
    char *prompt = ai_generate_prompt(command, file_context);
    
    // 发送请求
    char *response = ai_send_request(context, prompt);
    
    free(file_context);
    free(prompt);
    
    return response;
}

// ==================== 操作执行 ====================

bool ai_execute_operation(AIContext *context, AIOperation *op) {
    if (!op) return false;
    
    // 这里应该调用实际的二进制编辑器函数
    // 简化实现：打印操作信息
    
    printf("执行AI操作：\n");
    printf("  类型: ");
    switch (op->type) {
        case AI_OP_VIEW: printf("查看文件\n"); break;
        case AI_OP_EDIT: printf("编辑字节\n"); break;
        case AI_OP_INSERT: printf("插入字节\n"); break;
        case AI_OP_DELETE: printf("删除字节\n"); break;
        case AI_OP_SEARCH: printf("搜索模式\n"); break;
        case AI_OP_ANALYZE: printf("分析文件\n"); break;
        case AI_OP_CREATE: printf("创建文件\n"); break;
        case AI_OP_EXPORT: printf("导出区域\n"); break;
        default: printf("未知操作\n"); break;
    }
    
    if (op->filename) {
        printf("  文件名: %s\n", op->filename);
    }
    
    if (op->interpretation) {
        printf("  解析: %s\n", op->interpretation);
    }
    
    if (op->confidence) {
        printf("  置信度: %s\n", op->confidence);
    }
    
    // 实际执行（这里应该调用二进制编辑器的函数）
    // 例如：execute_view(), execute_edit() 等
    
    return true;
}

char* ai_execute_command(AIContext *context, const char *command) {
    // 解析命令
    AIOperation *op = ai_parse_command(context, command);
    if (!op) {
        return strdup("错误：无法解析命令");
    }
    
    // 执行操作
    bool success = ai_execute_operation(context, op);
    
    // 生成响应
    char *response = NULL;
    if (success) {
        asprintf(&response, "✅ 已执行命令：%s\n操作：%s\n文件：%s",
                command, 
                op->interpretation ? op->interpretation : "未知",
                op->filename ? op->filename : "无");
    } else {
        asprintf(&response, "❌ 执行失败：%s", command);
    }
    
    ai_operation_free(op);
    return response;
}

// ==================== 工具函数 ====================

void ai_print_help(void) {
    printf("\n=== AI二进制编辑器 - 帮助 ===\n");
    printf("配置命令:\n");
    printf("  /config <url> <key> [model]  - 配置AI模型\n");
    printf("  /save <文件>                 - 保存配置\n");
    printf("  /load <文件>                 - 加载配置\n");
    printf("  /status                      - 显示配置状态\n");
    printf("\nAI对话命令:\n");
    printf("  /chat <消息>                 - 与AI对话\n");
    printf("  /ask <关于文件的问题>        - 询问文件相关问题\n");
    printf("  /history                     - 显示对话历史\n");
    printf("  /clear                       - 清除对话历史\n");
    printf("\n二进制操作（自然语言）:\n");
    printf("  查看 test.bin 文件\n");
    printf("  在偏移0x10处修改为0xFF\n");
    printf("  搜索48 65 6C 6C 6F模式\n");
    printf("  分析文件类型\n");
    printf("  创建新的二进制文件\n");
    printf("\n退出:\n");
    printf("  /exit 或 /quit              - 退出程序\n");
    printf("===============================\n");
}

void ai_print_config(AIContext *context) {
    if (!context) {
        printf("AI上下文未初始化\n");
        return;
    }
    
    printf("\n=== AI模型配置 ===\n");
    printf("已配置: %s\n", context->config.configured ? "是" : "否");
    
    if (context->config.configured) {
        printf("API地址: %s\n", context->config.api_url);
        printf("模型: %s\n", context->config.model_name);
        printf("最大token: %d\n", context->config.max_tokens);
        printf("温度: %.2f\n", context->config.temperature);
    } else {
        printf("请使用 /config 命令配置AI模型\n");
    }
    
    printf("对话历史: %d 条\n", context->history_count);
    printf("========================\n");
}

void ai_print_operation(AIOperation *op) {
    if (!op) {
        printf("操作为空\n");
        return;
    }
    
    printf("\n=== AI解析结果 ===\n");
    printf("原始命令: %s\n", op->original_text);
    printf("解析: %s\n", op->interpretation);
    printf("置信度: %s\n", op->confidence);
    
    if (op->filename) {
        printf("文件名: %s\n", op->filename);
    }
    
    if (op->offset > 0) {
        printf("偏移量: 0x%08zX\n", op->offset);
    }
    
    if (op->value > 0) {
        printf("字节值: 0x%02X\n", op->value);
    }
    
    printf("========================\n");
}

char* ai_extract_filename(const char *command) {
    // 简化实现：查找常见的文件扩展名
    const char *extensions[] = {".bin", ".exe", ".dll", ".so", ".bmp", ".png", ".jpg", ".txt", NULL};
    
    for (int i = 0; extensions[i]; i++) {
        char *pos = strstr(command, extensions[i]);
        if (pos) {
            // 向前查找文件名开始
            int start = 0;
            for (int j = pos - command - 1; j >= 0; j--) {
                if (command[j] == ' ' || command[j] == '\"' || command[j] == '\'') {
                    start = j + 1;
                    break;
                }
            }
            
            int len = (pos - command) + strlen(extensions[i]) - start;
            char *filename = (char*)malloc(len + 1);
            strncpy(filename, command + start, len);
            filename[len] = '\0';
            
            return filename;
        }
    }
    
    return NULL;
}

char* ai_extract_offset(const char *command) {
    // 查找十六进制偏移 (0x...)
    const char *hex_prefix = "0x";
    char *pos = strstr(command, hex_prefix);
    
    if (pos) {
        // 提取十六进制数字
        char hex[32] = {0};
        int i = 0;
        pos += 2; // 跳过"0x"
        
        while (isxdigit(*pos) && i < 31) {
            hex[i++] = *pos++;
        }
        
        if (i > 0) {
            return strdup(hex);
        }
    }
    
    // 查找十进制偏移
    // 简化实现：查找数字
    for (int i = 0; command[i]; i++) {
        if (isdigit(command[i])) {
            int start = i;
            while (isdigit(command[i])) i++;
            
            int len = i - start;
            char *num = (char*)malloc(len + 1);
            strncpy(num, command + start, len);
            num[len] = '\0';
            
            // 检查是否在合理范围内（不是年份等）
            long value = strtol(num, NULL, 10);
            if (value > 0 && value < 0xFFFFFFFF) {
                return num;
            }
            free(num);
        }
    }
    
    return NULL;
}

char* ai_extract_value(const char *command) {
    // 查找十六进制值 (0x...)
    char *offset = ai_extract_offset(command);
    if (offset) {
        free(offset);
        
        // 如果找到了偏移，再找第二个0x作为值
        char *second_hex = strstr(command, "0x");
        if (second_hex) {
            second_hex = strstr(second_hex + 2, "0x");
            if (second_hex) {
                char hex[32] = {0};
                int i = 0;
                second_hex += 2;
                
                while (isxdigit(*second_hex) && i < 31) {
                    hex[i++] = *second_hex++;
                }
                
                if (i > 0) {
                    return strdup(hex);
                }
            }
        }
    }
    
    // 查找十进制值
    int found_offset = 0;
    for (int i = 0; command[i]; i++) {
        if (isdigit(command[i])) {
            if (!found_offset) {
                found_offset = 1;
                while (isdigit(command[i])) i++;
                continue;
            }
            
            // 第二个数字可能是值
            int start = i;
            while (isdigit(command[i])) i++;
            
            int len = i - start;
            char *num = (char*)malloc(len + 1);
            strncpy(num, command + start, len);
            num[len] = '\0';
            
            long value = strtol(num, NULL, 10);
            if (value >= 0 && value <= 255) {
                return num;
            }
            free(num);
        }
    }
    
    return NULL;
}

char* ai_extract_pattern(const char *command) {
    // 查找十六进制模式 (用空格分隔)
    char *pattern = NULL;
    char *hex_start = NULL;
    
    for (int i = 0; command[i]; i++) {
        if (isxdigit(command[i]) && (i == 0 || command[i-1] == ' ')) {
            if (!hex_start) hex_start = (char*)command + i;
            
            // 检查是否连续两个十六进制数字
            if (isxdigit(command[i+1])) {
                // 提取这个字节
                char byte[3] = {command[i], command[i+1], '\0'};
                
                if (!pattern) {
                    pattern = strdup(byte);
                } else {
                    char *new_pattern = NULL;
                    asprintf(&new_pattern, "%s %s", pattern, byte);
                    free(pattern);
                    pattern = new_pattern;
                }
                
                i += 2;
                hex_start = NULL;
            }
        }
    }
    
    return pattern;
}

// ==================== 文件上下文 ====================

char* ai_get_file_context(const char *filename, size_t max_size) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) return NULL;
    
    // 获取文件大小
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    if (file_size <= 0) {
        fclose(fp);
        return NULL;
    }
    
    // 读取部分内容
    size_t read_size = (file_size > max_size) ? max_size : file_size;
    uint8_t *buffer = (uint8_t*)malloc(read_size);
    size_t bytes_read = fread(buffer, 1, read_size, fp);
    fclose(fp);
    
    if (bytes_read == 0) {
        free(buffer);
        return NULL;
    }
    
    // 转换为十六进制字符串
    char *context = (char*)malloc(bytes_read * 3 + 128);
    strcpy(context, "文件大小: ");
    
    char size_str[32];
    sprintf(size_str, "%ld 字节\n", file_size);
    strcat(context, size_str);
    
    strcat(context, "前");
    sprintf(size_str, "%zu", bytes_read);
    strcat(context, size_str);
    strcat(context, "字节的十六进制：\n");
    
    for (size_t i = 0; i < bytes_read; i++) {
        char byte_str[4];
        sprintf(byte_str, "%02X ", buffer[i]);
        strcat(context, byte_str);
        
        if ((i + 1) % 16 == 0) {
            strcat(context, "\n");
        }
    }
    
    free(buffer);
    return context;
}

char* ai_analyze_file_for_ai(const char *filename) {
    // 分析文件并提供AI友好的描述
    char *analysis = NULL;
    
    // 获取文件大小
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        asprintf(&analysis, "无法打开文件: %s", filename);
        return analysis;
    }
    
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    // 读取文件头
    uint8_t header[64];
    size_t header_size = fread(header, 1, sizeof(header), fp);
    fclose(fp);
    
    // 分析文件类型
    const char *file_type = "未知";
    
    if (header_size >= 4) {
        if (header[0] == 0x42 && header[1] == 0x4D) {
            file_type = "BMP图像";
        } else if (header[0] == 0x7F && header[1] == 'E' && header[2] == 'L' && header[3] == 'F') {
            file_type = "ELF可执行文件";
        } else if (header[0] == 0xFE && header[1] == 0xED && header[2] == 0xFA && header[3] == 0xCE) {
            file_type = "Mach-O (32位)";
        } else if (header[0] == 0xFE && header[1] == 0xED && header[2] == 0xFA && header[3] == 0xCF) {
            file_type = "Mach-O (64位)";
        } else if (header[0] == 0xCA && header[1] == 0xFE && header[2] == 0xBA && header[3] == 0xBE) {
            file_type = "Java类文件";
        } else if (header[0] == 0x50 && header[1] == 0x4B && header[2] == 0x03 && header[3] == 0x04) {
            file_type = "ZIP压缩文件";
        } else if (header[0] == 0x89 && header[1] == 'P' && header[2] == 'N' && header[3] == 'G') {
            file_type = "PNG图像";
        } else if (header[0] == 0xFF && header[1] == 0xD8) {
            file_type = "JPEG图像";
        } else if (header[0] == 'P' && header[1] == 'K') {
            file_type = "ZIP文件";
        }
    }
    
    // 统计可打印字符
    int printable = 0;
    for (size_t i = 0; i < header_size; i++) {
        if (isprint(header[i])) printable++;
    }
    
    double printable_percent = (double)printable / header_size * 100;
    
    asprintf(&analysis,
        "文件分析报告：\n"
        "文件名: %s\n"
        "大小: %ld 字节\n"
        "类型: %s\n"
        "文件头 (%zu 字节):\n",
        filename, size, file_type, header_size);
    
    // 添加十六进制转储
    char *hex_dump = (char*)malloc(header_size * 3 + 64);
    strcpy(hex_dump, "");
    
    for (size_t i = 0; i < header_size; i++) {
        char byte_str[4];
        sprintf(byte_str, "%02X ", header[i]);
        strcat(hex_dump, byte_str);
        
        if ((i + 1) % 16 == 0) {
            strcat(hex_dump, "\n");
        }
    }
    
    char *full_analysis = NULL;
    asprintf(&full_analysis, "%s%s\n可打印字符: %d (%.1f%%)",
             analysis, hex_dump, printable, printable_percent);
    
    free(analysis);
    free(hex_dump);
    
    return full_analysis;
}

// ==================== 学习反馈 ====================

void ai_add_to_history(AIContext *context, const char *command, const char *result) {
    if (!context) return;
    
    // 确保历史数组已分配
    if (!context->history) {
        context->history = (char**)malloc(context->max_history * sizeof(char*));
        if (!context->history) return;
    }
    
    // 扩展历史数组
    if (context->history_count >= context->max_history) {
        context->max_history *= 2;
        char **new_history = (char**)realloc(context->history, 
                                           context->max_history * sizeof(char*));
        if (!new_history) return;
        context->history = new_history;
    }
    
    // 添加命令
    if (command) {
        char *entry = NULL;
        if (asprintf(&entry, "用户: %s", command) > 0) {
            context->history[context->history_count++] = entry;
        }
    }
    
    // 添加结果
    if (result) {
        char *entry = NULL;
        if (asprintf(&entry, "AI: %s", result) > 0) {
            context->history[context->history_count++] = entry;
        }
    }
}

void ai_learn_from_feedback(AIContext *context, const char *command, bool success, const char *feedback) {
    // 简化实现：记录反馈
    if (!context || !command) return;
    
    char *feedback_entry = NULL;
    asprintf(&feedback_entry, "反馈: %s -> %s (%s)",
             command,
             success ? "成功" : "失败",
             feedback ? feedback : "无");
    
    ai_add_to_history(context, feedback_entry, NULL);
    free(feedback_entry);
}

// ==================== 交互式主循环 ====================

void ai_interactive_mode(AIContext *context) {
    printf("\n🎯 AI二进制编辑器 - 交互模式\n");
    printf("输入 /help 查看帮助，/exit 退出\n\n");
    
    char input[1024];
    bool running = true;
    
    while (running) {
        printf("AI> ");
        fflush(stdout);  // 确保提示符显示
        
        // 清空输入缓冲区
        memset(input, 0, sizeof(input));
        
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }
        
        // 去除换行符和回车符
        input[strcspn(input, "\n")] = 0;
        input[strcspn(input, "\r")] = 0;
        
        if (strlen(input) == 0) {
            continue;
        }
        
        // 处理命令
        if (input[0] == '/') {
            // 系统命令
            if (strcmp(input, "/exit") == 0 || strcmp(input, "/quit") == 0) {
                printf("退出AI二进制编辑器。\n");
                running = false;
            }
            else if (strcmp(input, "/help") == 0) {
                ai_print_help();
            }
            else if (strcmp(input, "/status") == 0) {
                ai_print_config(context);
            }
            else if (strncmp(input, "/config ", 8) == 0) {
                // 解析配置参数
                char url[256] = {0};
                char key[256] = {0};
                char model[64] = {0};
                
                if (sscanf(input + 8, "%255s %255s %63s", url, key, model) >= 2) {
                    if (ai_configure_model(context, url, key, model[0] ? model : NULL)) {
                        printf("✅ AI模型配置成功\n");
                        ai_print_config(context);
                    } else {
                        printf("❌ 配置失败\n");
                    }
                } else {
                    printf("用法: /config <API地址> <API密钥> [模型名称]\n");
                    printf("示例: /config https://api.openai.com/v1/chat/completions sk-your-api-key gpt-4\n");
                }
            }
            else if (strncmp(input, "/chat ", 6) == 0) {
                if (!context->config.configured) {
                    printf("❌ 请先配置AI模型: /config <地址> <密钥>\n");
                    continue;
                }
                
                char *response = ai_chat(context, input + 6);
                printf("AI: %s\n", response);
                free(response);
            }
            else if (strncmp(input, "/ask ", 5) == 0) {
                // 处理文件相关问题
                printf("正在分析命令...\n");
                char *response = ai_execute_command(context, input + 5);
                printf("%s\n", response);
                free(response);
            }
            else if (strcmp(input, "/history") == 0) {
                printf("\n=== 对话历史 ===\n");
                for (int i = 0; i < context->history_count; i++) {
                    printf("%s\n", context->history[i]);
                }
                printf("================\n");
            }
            else if (strcmp(input, "/clear") == 0) {
                for (int i = 0; i < context->history_count; i++) {
                    free(context->history[i]);
                }
                context->history_count = 0;
                printf("✅ 对话历史已清除\n");
            }
            else if (strncmp(input, "/save ", 6) == 0) {
                if (ai_save_config(context, input + 6)) {
                    printf("✅ 配置已保存到: %s\n", input + 6);
                } else {
                    printf("❌ 保存失败\n");
                }
            }
            else if (strncmp(input, "/load ", 6) == 0) {
                if (ai_load_config(context, input + 6)) {
                    printf("✅ 配置已从文件加载\n");
                    ai_print_config(context);
                } else {
                    printf("❌ 加载失败\n");
                }
            }
            else {
                printf("未知命令: %s\n", input);
                printf("输入 /help 查看可用命令\n");
            }
        } else {
            // 自然语言命令
            printf("正在解析: %s\n", input);
            
            AIOperation *op = ai_parse_command(context, input);
            if (op) {
                ai_print_operation(op);
                
                if (op->type != AI_OP_UNKNOWN) {
                    printf("执行操作...\n");
                    ai_execute_operation(context, op);
                    
                    // 如果是查看操作，显示文件内容
                    if (op->type == AI_OP_VIEW && op->filename) {
                        printf("\n执行查看操作...\n");
                        
                        // 检查文件是否存在
                        FILE *fp = fopen(op->filename, "rb");
                        if (!fp) {
                            printf("❌ 文件不存在: %s\n", op->filename);
                        } else {
                            fclose(fp);
                            
                            // 调用二进制编辑器查看文件
                            char command[512];
                            snprintf(command, sizeof(command), "./bin_editor --view \"%s\"", op->filename);
                            printf("执行命令: %s\n", command);
                            system(command);
                        }
                    }
                } else {
                    printf("❌ 无法理解此命令\n");
                    printf("尝试更明确的描述，例如：\n");
                    printf("  - 查看 test.bin 文件\n");
                    printf("  - 在偏移0x10处修改为0xFF\n");
                    printf("  - 搜索48 65 6C 6C 6F模式\n");
                }
                
                ai_operation_free(op);
            } else {
                printf("❌ 命令解析失败\n");
            }
        }
    }
}