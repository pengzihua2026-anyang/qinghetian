//
//  ai_integration.h
//  AI大模型集成接口
//
//  功能：
//  1. 配置AI模型（地址和Key）
//  2. 自然语言对话解析为二进制操作
//  3. 执行AI指令并操作二进制文件
//  4. 学习反馈机制
//

#ifndef AI_INTEGRATION_H
#define AI_INTEGRATION_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

// AI模型配置
typedef struct {
    char *api_url;          // API地址
    char *api_key;          // API密钥
    char *model_name;       // 模型名称
    int max_tokens;         // 最大token数
    double temperature;     // 温度参数
    bool configured;        // 是否已配置
} AIModelConfig;

// AI对话上下文
typedef struct {
    AIModelConfig config;   // 模型配置
    char *conversation_id;  // 对话ID
    char **history;         // 对话历史
    int history_count;      // 历史记录数
    int max_history;        // 最大历史记录
} AIContext;

// AI操作类型
typedef enum {
    AI_OP_VIEW,             // 查看文件
    AI_OP_EDIT,             // 编辑字节
    AI_OP_INSERT,           // 插入字节
    AI_OP_DELETE,           // 删除字节
    AI_OP_SEARCH,           // 搜索模式
    AI_OP_EXPORT,           // 导出区域
    AI_OP_ANALYZE,          // 分析文件
    AI_OP_CREATE,           // 创建文件
    AI_OP_COMPARE,          // 比较文件
    AI_OP_HEX_TO_ASCII,     // 十六进制转ASCII
    AI_OP_ASCII_TO_HEX,     // ASCII转十六进制
    AI_OP_UNKNOWN           // 未知操作
} AIOperationType;

// AI解析结果
typedef struct {
    AIOperationType type;   // 操作类型
    char *filename;         // 文件名
    char *target_file;      // 目标文件（用于导出/比较）
    
    // 操作参数
    size_t offset;          // 偏移量
    uint8_t value;          // 字节值
    uint8_t *pattern;       // 搜索模式
    size_t pattern_len;     // 模式长度
    size_t start_offset;    // 起始偏移
    size_t end_offset;      // 结束偏移
    size_t size;            // 大小（用于创建）
    
    // 自然语言
    char *original_text;    // 原始文本
    char *interpretation;   // 解析结果
    char *confidence;       // 置信度
} AIOperation;

// 函数声明

// 配置管理
AIContext* ai_context_create(void);
void ai_context_free(AIContext *context);
bool ai_configure_model(AIContext *context, const char *url, const char *key, const char *model);
bool ai_load_config(AIContext *context, const char *config_file);
bool ai_save_config(AIContext *context, const char *config_file);

// 自然语言解析
AIOperation* ai_parse_command(AIContext *context, const char *command);
void ai_operation_free(AIOperation *op);
char* ai_generate_prompt(const char *command, const char *file_context);

// AI通信
char* ai_send_request(AIContext *context, const char *prompt);
char* ai_chat(AIContext *context, const char *message);
char* ai_process_binary_command(AIContext *context, const char *command, const char *filename);

// 操作执行
bool ai_execute_operation(AIContext *context, AIOperation *op);
char* ai_execute_command(AIContext *context, const char *command);

// 工具函数
void ai_print_help(void);
void ai_print_config(AIContext *context);
void ai_print_operation(AIOperation *op);
char* ai_extract_filename(const char *command);
char* ai_extract_offset(const char *command);
char* ai_extract_value(const char *command);
char* ai_extract_pattern(const char *command);

// 文件上下文
char* ai_get_file_context(const char *filename, size_t max_size);
char* ai_analyze_file_for_ai(const char *filename);

// 学习反馈
void ai_add_to_history(AIContext *context, const char *command, const char *result);
void ai_learn_from_feedback(AIContext *context, const char *command, bool success, const char *feedback);

// 交互模式
void ai_interactive_mode(AIContext *context);

// 错误处理
#define AI_ERROR_NONE 0
#define AI_ERROR_CONFIG 1
#define AI_ERROR_PARSE 2
#define AI_ERROR_EXECUTE 3
#define AI_ERROR_NETWORK 4
#define AI_ERROR_MEMORY 5

// JSON解析辅助函数（在ai_integration.c中实现）
char* extract_ai_response(const char *json_response);

#endif /* AI_INTEGRATION_H */