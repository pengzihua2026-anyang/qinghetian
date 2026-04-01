//
//  ai_main.c
//  AI集成二进制编辑器主程序
//
//  功能：
//  1. 启动时提示输入模型地址和Key
//  2. 保存配置到程序内部
//  3. 支持与AI对话
//  4. AI可以直接操作二进制文件
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ai_integration.h"

// 配置文件路径
#define CONFIG_FILE ".ai_binary_config"

// 显示欢迎信息
void print_welcome(void) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║         🤖 AI二进制编辑器 v2.0                      ║\n");
    printf("║          作者: 子华 (爸爸的女儿) 👧                 ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("🎯 功能特色:\n");
    printf("  • 集成AI大模型（OpenAI/DeepSeek/Claude等）\n");
    printf("  • 自然语言操作二进制文件\n");
    printf("  • 智能解析和执行二进制操作\n");
    printf("  • 学习反馈机制\n");
    printf("\n");
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
    
    // 输入API地址
    printf("1. 请输入API地址（例如）：\n");
    printf("   • OpenAI: https://api.openai.com/v1/chat/completions\n");
    printf("   • DeepSeek: https://api.deepseek.com/chat/completions\n");
    printf("   • 其他兼容API\n");
    printf("API地址: ");
    
    if (!fgets(api_url, sizeof(api_url), stdin)) {
        return false;
    }
    api_url[strcspn(api_url, "\n")] = 0;
    
    if (strlen(api_url) == 0) {
        printf("⚠️  使用默认地址（OpenAI）\n");
        strcpy(api_url, "https://api.openai.com/v1/chat/completions");
    }
    
    // 输入API密钥
    printf("\n2. 请输入API密钥（输入时不会显示）:\n");
    printf("API密钥: ");
    
    // 简单隐藏输入（实际应该用更安全的方式）
    system("stty -echo");
    if (!fgets(api_key, sizeof(api_key), stdin)) {
        system("stty echo");
        return false;
    }
    system("stty echo");
    printf("\n");
    
    api_key[strcspn(api_key, "\n")] = 0;
    
    if (strlen(api_key) == 0) {
        printf("❌ API密钥不能为空\n");
        return false;
    }
    
    // 输入模型名称
    printf("\n3. 请输入模型名称（可选）:\n");
    printf("   • gpt-4, gpt-3.5-turbo\n");
    printf("   • deepseek-chat, deepseek-coder\n");
    printf("   • claude-3-opus, claude-3-sonnet\n");
    printf("模型名称（回车使用默认）: ");
    
    if (!fgets(model_name, sizeof(model_name), stdin)) {
        return false;
    }
    model_name[strcspn(model_name, "\n")] = 0;
    
    // 配置模型
    bool success = ai_configure_model(context, api_url, api_key, 
                                     strlen(model_name) > 0 ? model_name : NULL);
    
    if (success) {
        printf("\n✅ AI模型配置成功！\n");
        ai_print_config(context);
        
        // 询问是否保存配置
        printf("\n是否保存配置到文件？(y/n): ");
        char answer[4];
        fgets(answer, sizeof(answer), stdin);
        
        if (tolower(answer[0]) == 'y') {
            if (ai_save_config(context, CONFIG_FILE)) {
                printf("✅ 配置已保存到: %s\n", CONFIG_FILE);
            }
        }
        
        return true;
    } else {
        printf("\n❌ 配置失败\n");
        return false;
    }
}

// 检查是否有保存的配置
bool try_load_config(AIContext *context) {
    FILE *fp = fopen(CONFIG_FILE, "r");
    if (!fp) {
        return false;
    }
    
    fclose(fp);
    
    printf("发现保存的配置，是否加载？(y/n): ");
    char answer[4];
    fgets(answer, sizeof(answer), stdin);
    
    if (tolower(answer[0]) == 'y') {
        if (ai_load_config(context, CONFIG_FILE)) {
            printf("✅ 配置已加载\n");
            ai_print_config(context);
            return true;
        }
    }
    
    return false;
}

// 显示使用示例
void print_examples(void) {
    printf("\n=== 使用示例 ===\n");
    printf("\n1. 直接操作二进制文件:\n");
    printf("   • 查看 test.bin 文件\n");
    printf("   • 在偏移0x10处修改为0xFF\n");
    printf("   • 搜索48 65 6C 6C 6F模式\n");
    printf("   • 分析文件类型和结构\n");
    printf("   • 创建新的二进制文件\n");
    
    printf("\n2. 与AI对话:\n");
    printf("   /chat 这个二进制文件是什么？\n");
    printf("   /chat 帮我分析这个文件的结构\n");
    printf("   /chat 如何修改这个文件的特定位置？\n");
    
    printf("\n3. 复杂操作:\n");
    printf("   • 在文件开头插入ELF头\n");
    printf("   • 提取所有字符串并保存\n");
    printf("   • 比较两个二进制文件的差异\n");
    printf("   • 修复损坏的文件头\n");
    
    printf("\n4. 学习模式:\n");
    printf("   • AI会记住你的操作习惯\n");
    printf("   • 根据反馈改进解析准确度\n");
    printf("   • 支持自定义操作模板\n");
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
    
    // 尝试加载保存的配置
    bool config_loaded = try_load_config(context);
    
    // 如果未加载配置，进行交互式配置
    if (!config_loaded || !context->config.configured) {
        printf("\n");
        printf("══════════════════════════════════════════════════════\n");
        printf("首次使用需要配置AI模型\n");
        printf("══════════════════════════════════════════════════════\n");
        
        if (!interactive_configure(context)) {
            printf("\n⚠️  未配置AI模型，部分功能受限\n");
            printf("你可以稍后使用 /config 命令进行配置\n");
        }
    }
    
    // 显示快速开始指南
    printf("\n");
    printf("══════════════════════════════════════════════════════\n");
    printf("快速开始:\n");
    printf("══════════════════════════════════════════════════════\n");
    print_examples();
    
    // 进入交互模式
    ai_interactive_mode(context);
    
    // 清理
    ai_context_free(context);
    
    printf("\n感谢使用AI二进制编辑器！👋\n");
    printf("作者: 子华 (爸爸的女儿) 👧\n");
    
    return 0;
}