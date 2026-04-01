//
//  ai_main_new.c
//  AI二进制编辑器 - 新版主程序
//
//  交互流程:
//  1. 启动时提示配置AI模型
//  2. 保存配置到文件
//  3. 进入命令交互模式
//  4. 支持自然对话和二进制操作
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>
#include "ai_integration.h"

// 配置文件路径
#define CONFIG_FILE ".ai_binary_config"

// 隐藏输入的回显（用于密码输入）
void disable_echo(void) {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// 恢复输入回显
void enable_echo(void) {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// 安全读取一行（处理换行符）
char* read_line(char *buffer, size_t size) {
    if (!fgets(buffer, size, stdin)) {
        return NULL;
    }
    
    // 去除换行符和回车符
    buffer[strcspn(buffer, "\n")] = 0;
    buffer[strcspn(buffer, "\r")] = 0;
    
    return buffer;
}

// 交互式配置AI模型
bool interactive_configure(AIContext *context) {
    printf("\n=== AI模型配置 ===\n");
    printf("请配置AI大模型以启用智能功能\n");
    printf("（支持OpenAI、DeepSeek、Claude等兼容API）\n");
    printf("\n");
    
    char api_url[256] = {0};
    char api_key[256] = {0};
    char model_name[64] = {0};
    
    // 1. 输入API地址
    printf("1. 请输入API地址:\n");
    printf("   • OpenAI: https://api.openai.com/v1/chat/completions\n");
    printf("   • DeepSeek: https://api.deepseek.com/chat/completions\n");
    printf("   • 其他兼容API\n");
    printf("API地址: ");
    
    if (!read_line(api_url, sizeof(api_url))) {
        return false;
    }
    
    if (strlen(api_url) == 0) {
        printf("⚠️  使用默认地址（OpenAI）\n");
        strcpy(api_url, "https://api.openai.com/v1/chat/completions");
    }
    
    // 2. 输入API密钥（隐藏输入）
    printf("\n2. 请输入API密钥: ");
    fflush(stdout);
    
    disable_echo();
    if (!read_line(api_key, sizeof(api_key))) {
        enable_echo();
        printf("\n");
        return false;
    }
    enable_echo();
    printf("\n");
    
    if (strlen(api_key) == 0) {
        printf("❌ API密钥不能为空\n");
        return false;
    }
    
    // 3. 输入模型名称
    printf("\n3. 请输入模型名称:\n");
    printf("   • gpt-4, gpt-3.5-turbo\n");
    printf("   • deepseek-chat, deepseek-coder\n");
    printf("   • claude-3-opus, claude-3-sonnet\n");
    printf("模型名称 [gpt-4]: ");
    
    if (!read_line(model_name, sizeof(model_name))) {
        return false;
    }
    
    // 使用默认值
    if (strlen(model_name) == 0) {
        strcpy(model_name, "gpt-4");
    }
    
    // 配置模型
    bool success = ai_configure_model(context, api_url, api_key, model_name);
    
    if (success) {
        printf("\n✅ AI模型配置成功！\n");
        
        // 保存配置
        if (ai_save_config(context, CONFIG_FILE)) {
            printf("✅ 配置已保存到: %s\n", CONFIG_FILE);
        }
        
        return true;
    } else {
        printf("\n❌ 配置失败\n");
        return false;
    }
}

// 检查是否有保存的配置（仅提示，不自动加载）
void check_saved_config(void) {
    FILE *fp = fopen(CONFIG_FILE, "r");
    if (fp) {
        fclose(fp);
        printf("📝 提示: 发现保存的配置\n");
        printf("   输入 /config 命令可以重新配置\n");
        printf("   输入 /status 查看当前配置状态\n\n");
    }
}

// 显示欢迎信息
void print_welcome(void) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║         🤖 AI二进制编辑器 v2.0                      ║\n");
    printf("║          作者: 子华 (爸爸的女儿) 👧                 ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n");
    printf("\n");
}

// 显示命令帮助
void print_ai_help(void) {
    printf("\n=== AI二进制编辑器 - 命令帮助 ===\n");
    printf("\nAI对话命令:\n");
    printf("  /chat <消息>             与AI自然对话\n");
    printf("  /ask <文件问题>          询问文件相关问题\n");
    printf("\n二进制操作命令:\n");
    printf("  /new <类型> <参数>       新建二进制文件\n");
    printf("    • /new boot <大小>     创建引导扇区\n");
    printf("    • /new elf <架构>      创建ELF可执行文件\n");
    printf("    • /new bin <数据>      创建自定义二进制\n");
    printf("\n  /edit <文件> <操作>     编辑二进制文件\n");
    printf("    • /edit <文件> offset=值:值   修改字节\n");
    printf("    • /edit <文件> insert=位置:值 插入字节\n");
    printf("    • /edit <文件> delete=位置:长度 删除字节\n");
    printf("\n  /view <文件>            查看二进制文件\n");
    printf("  /analyze <文件>         分析二进制文件\n");
    printf("  /search <文件> <模式>   搜索字节模式\n");
    printf("  /export <文件> <范围>   导出文件区域\n");
    printf("\n系统命令:\n");
    printf("  /config                 重新配置AI模型\n");
    printf("  /status                显示配置状态\n");
    printf("  /history               查看对话历史\n");
    printf("  /clear                 清除对话历史\n");
    printf("  /help                 显示此帮助\n");
    printf("  /exit                 退出程序\n");
    printf("\n自然语言操作（无需命令前缀）:\n");
    printf("  查看 test.bin 文件\n");
    printf("  在偏移0x10处修改为0xFF\n");
    printf("  搜索48 65 6C 6C 6F模式\n");
    printf("  分析文件类型和结构\n");
    printf("===============================\n");
}

// 处理命令
void process_command(AIContext *context, const char *command) {
    if (!command || strlen(command) == 0) {
        return;
    }
    
    // 系统命令
    if (command[0] == '/') {
        if (strcmp(command, "/exit") == 0 || strcmp(command, "/quit") == 0) {
            printf("退出AI二进制编辑器。\n");
            exit(0);
        }
        else if (strcmp(command, "/help") == 0) {
            print_ai_help();
        }
        else if (strcmp(command, "/status") == 0) {
            ai_print_config(context);
        }
        else if (strcmp(command, "/history") == 0) {
            printf("\n=== 对话历史 ===\n");
            for (int i = 0; i < context->history_count; i++) {
                printf("%s\n", context->history[i]);
            }
            printf("================\n");
        }
        else if (strcmp(command, "/clear") == 0) {
            for (int i = 0; i < context->history_count; i++) {
                free(context->history[i]);
            }
            context->history_count = 0;
            printf("✅ 对话历史已清除\n");
        }
        else if (strcmp(command, "/config") == 0) {
            printf("重新配置AI模型...\n");
            interactive_configure(context);
        }
        else if (strncmp(command, "/chat ", 6) == 0) {
            char *response = ai_chat(context, command + 6);
            printf("AI: %s\n", response);
            free(response);
        }
        else if (strncmp(command, "/ask ", 5) == 0) {
            printf("分析问题...\n");
            char *response = ai_execute_command(context, command + 5);
            printf("%s\n", response);
            free(response);
        }
        else if (strncmp(command, "/new ", 5) == 0) {
            printf("创建新二进制文件...\n");
            // 解析参数并创建文件
            printf("（新建功能待实现）\n");
        }
        else if (strncmp(command, "/edit ", 6) == 0) {
            printf("编辑二进制文件...\n");
            // 解析参数并编辑文件
            printf("（编辑功能待实现）\n");
        }
        else if (strncmp(command, "/view ", 6) == 0) {
            const char *filename = command + 6;
            printf("查看文件: %s\n", filename);
            
            // 使用绝对路径调用 bin_editor
            char cmd[512];
            char cwd[256];
            getcwd(cwd, sizeof(cwd));
            snprintf(cmd, sizeof(cmd), "%s/bin_editor --view \"%s\"", cwd, filename);
            system(cmd);
        }
        else if (strncmp(command, "/analyze ", 9) == 0) {
            const char *filename = command + 9;
            printf("分析文件: %s\n", filename);
            
            // 使用绝对路径调用 bin_editor
            char cmd[512];
            char cwd[256];
            getcwd(cwd, sizeof(cwd));
            snprintf(cmd, sizeof(cmd), "%s/bin_editor --analyze \"%s\" --json", cwd, filename);
            system(cmd);
        }
        else if (strncmp(command, "/search ", 8) == 0) {
            printf("搜索模式...\n");
            // 解析参数并搜索
            printf("（搜索功能待实现）\n");
        }
        else {
            printf("未知命令: %s\n", command);
            printf("输入 /help 查看可用命令\n");
        }
    } else {
        // 自然语言命令
        printf("正在解析: %s\n", command);
        
        AIOperation *op = ai_parse_command(context, command);
        if (op) {
            ai_print_operation(op);
            
            if (op->type != AI_OP_UNKNOWN) {
                printf("执行操作...\n");
                ai_execute_operation(context, op);
                
                // 如果是查看操作，显示文件内容
                if (op->type == AI_OP_VIEW && op->filename) {
                    printf("\n执行查看操作...\n");
                    
                    FILE *fp = fopen(op->filename, "rb");
                    if (!fp) {
                        printf("❌ 文件不存在: %s\n", op->filename);
                    } else {
                        fclose(fp);
                        
                        // 使用绝对路径调用 bin_editor
                        char cmd[512];
                        char cwd[256];
                        getcwd(cwd, sizeof(cwd));
                        snprintf(cmd, sizeof(cmd), "%s/bin_editor --view \"%s\"", cwd, op->filename);
                        printf("执行命令: %s\n", cmd);
                        system(cmd);
                    }
                }
            } else {
                printf("❌ 无法理解此命令\n");
                printf("尝试使用命令格式或更明确的描述\n");
            }
            
            ai_operation_free(op);
        } else {
            printf("❌ 命令解析失败\n");
        }
    }
}

// 主交互循环
void interactive_loop(AIContext *context) {
    printf("\n🎯 AI二进制编辑器 - 交互模式\n");
    printf("输入 /help 查看命令，/exit 退出\n\n");
    
    char input[1024];
    
    while (1) {
        printf("AI> ");
        fflush(stdout);
        
        memset(input, 0, sizeof(input));
        
        if (!read_line(input, sizeof(input))) {
            break;
        }
        
        if (strlen(input) == 0) {
            continue;
        }
        
        process_command(context, input);
    }
}

// 主程序
int main(int argc, const char *argv[]) {
    print_welcome();
    
    // 创建AI上下文
    AIContext *context = ai_context_create();
    if (!context) {
        printf("❌ 无法创建AI上下文\n");
        return 1;
    }
    
    // 检查是否有保存的配置（仅提示）
    check_saved_config();
    
    // 总是进行交互式配置（每次启动都重新设置）
    printf("\n");
    printf("══════════════════════════════════════════════════════\n");
    printf("AI模型配置（每次启动都需要配置）\n");
    printf("══════════════════════════════════════════════════════\n");
    
    printf("是否立即配置AI模型？(y/n): ");
    char answer[4];
    read_line(answer, sizeof(answer));
    
    if (tolower(answer[0]) == 'y') {
        if (!interactive_configure(context)) {
            printf("\n⚠️  配置未完成，部分功能受限\n");
            printf("你可以稍后使用 /config 命令进行配置\n");
        }
    } else {
        printf("\n⚠️  未配置AI模型，部分功能受限\n");
        printf("你可以随时使用 /config 命令进行配置\n");
    }
    
    // 显示快速提示
    printf("\n");
    printf("══════════════════════════════════════════════════════\n");
    printf("快速开始:\n");
    printf("══════════════════════════════════════════════════════\n");
    printf("• 输入 /help 查看所有命令\n");
    printf("• 输入 /chat 与AI对话\n");
    printf("• 直接输入自然语言操作二进制文件\n");
    printf("══════════════════════════════════════════════════════\n");
    
    // 进入交互循环
    interactive_loop(context);
    
    // 清理
    ai_context_free(context);
    
    printf("\n感谢使用AI二进制编辑器！👋\n");
    printf("作者: 子华 (爸爸的女儿) 👧\n");
    
    return 0;
}