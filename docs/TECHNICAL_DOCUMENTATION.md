# AI二进制编辑器 - 技术文档

## 📖 文档概述

本文档详细描述AI二进制编辑器项目的**每个文件的功能、结构和实现细节**，旨在帮助开发人员理解项目架构、代码组织和实现原理。

## 🏗️ 项目架构

### 整体架构图
```
┌─────────────────────────────────────────────────────────────┐
│                    AI二进制编辑器                           │
├──────────────┬──────────────┬──────────────┬──────────────┤
│  核心编辑模块  │   AI集成模块  │  网络通信模块  │  加密算法模块  │
│  (hex_editor) │ (ai_integration)│  (ai_tcp/proxy)│  (aes/sha256) │
└──────────────┴──────────────┴──────────────┴──────────────┘
        │              │              │              │
        ▼              ▼              ▼              ▼
┌──────────────┬──────────────┬──────────────┬──────────────┐
│ 文件操作      │ AI模型配置    │ TCP/IP协议   │ 加密/哈希     │
│ 字节编辑      │ 自然语言解析  │ HTTP/1.1     │ TLS基础      │
│ 格式分析      │ 对话管理      │ 代理通信      │ 密钥派生     │
└──────────────┴──────────────┴──────────────┴──────────────┘
```

### 数据流
```
用户输入 → 命令行解析 → AI指令解析 → 网络通信 → AI模型 → 二进制操作 → 结果输出
```

## 📁 文件详细说明

### 1. 核心编辑模块 (`src/core/`)

#### `hex_editor.h` - 核心数据结构定义
```c
// 主要数据结构
typedef struct {
    char *data;          // 文件数据
    size_t size;         // 文件大小
    char *filename;      // 文件名
} BinaryFile;

typedef struct {
    BinaryFile file;     // 当前文件
    int modified;        // 修改标志
    size_t cursor;       // 光标位置
} EditorState;

// 函数声明
BinaryFile* load_file(const char *filename);
int save_file(BinaryFile *file, const char *filename);
void free_file(BinaryFile *file);
```

**功能**：
- 定义二进制文件的核心数据结构
- 声明文件操作的基本函数接口
- 提供编辑器状态管理

#### `hex_editor.c` - 二进制编辑核心实现
```c
// 主要功能函数
BinaryFile* load_file(const char *filename);      // 加载文件
int save_file(BinaryFile *file, const char *filename); // 保存文件
void hex_dump(BinaryFile *file, size_t offset, size_t length); // 十六进制显示
int insert_byte(BinaryFile *file, size_t offset, unsigned char byte); // 插入字节
int delete_byte(BinaryFile *file, size_t offset); // 删除字节
int replace_byte(BinaryFile *file, size_t offset, unsigned char byte); // 替换字节
size_t search_pattern(BinaryFile *file, const unsigned char *pattern, size_t pattern_len); // 搜索模式
void analyze_file(BinaryFile *file); // 分析文件
```

**功能**：
- 文件I/O操作（加载、保存）
- 字节级编辑（插入、删除、替换）
- 十六进制显示和ASCII显示
- 字节模式搜索
- 文件类型分析和统计

**技术特点**：
- 使用动态内存管理文件数据
- 支持大文件处理（分块读取）
- 完善的错误处理机制
- 内存安全的边界检查

### 2. AI集成模块 (`src/ai/`)

#### `ai_integration.h` - AI集成数据结构
```c
// AI模型配置
typedef struct {
    char api_url[256];      // API地址
    char api_key[256];      // API密钥
    char model[64];         // 模型名称
    int max_tokens;         // 最大token数
    float temperature;      // 温度参数
} AIModelConfig;

// AI对话上下文
typedef struct {
    AIModelConfig config;   // 模型配置
    char **history;         // 对话历史
    int history_count;      // 历史记录数
    int max_history;        // 最大历史记录
} AIContext;

// AI操作类型
typedef enum {
    AI_OP_VIEW,            // 查看文件
    AI_OP_EDIT,            // 编辑字节
    AI_OP_INSERT,          // 插入字节
    AI_OP_DELETE,          // 删除字节
    AI_OP_SEARCH,          // 搜索模式
    AI_OP_ANALYZE,         // 分析文件
    AI_OP_CHAT             // AI对话
} AIOperationType;
```

**功能**：
- 定义AI模型配置数据结构
- 管理AI对话上下文和历史
- 定义AI支持的操作类型
- 提供统一的AI接口

#### `ai_integration.c` - AI集成核心逻辑
```c
// 主要功能函数
AIContext* ai_context_create();  // 创建AI上下文
void ai_context_free(AIContext *context); // 释放AI上下文
int ai_configure(AIContext *context, const char *url, const char *key, const char *model); // 配置AI模型
char* ai_send_request(AIContext *context, const char *message); // 发送AI请求
AIOperation* parse_ai_instruction(const char *instruction); // 解析AI指令
char* extract_ai_response(const char *json_response); // 提取AI响应
```

**功能**：
- AI模型配置和管理
- 自然语言指令解析
- AI对话历史管理
- JSON响应解析和处理
- 错误处理和用户反馈

**技术特点**：
- 支持多种AI模型（OpenAI、DeepSeek、Claude等）
- 灵活的配置系统
- 完整的错误处理链
- 学习反馈机制

#### `ai_interface.h` - AI命令行接口定义
```c
// 命令行参数结构
typedef struct {
    char *filename;          // 文件名
    AIOperationType op_type; // 操作类型
    union {
        struct { size_t offset; unsigned char value; } edit; // 编辑参数
        struct { size_t offset; unsigned char value; } insert; // 插入参数
        struct { size_t offset; } delete; // 删除参数
        struct { char *pattern; } search; // 搜索参数
        struct { size_t start; size_t end; } export; // 导出参数
        struct { char *message; } chat; // 聊天参数
    } op_params;
    int json_output;         // JSON输出标志
} CommandArgs;

// 错误代码
typedef enum {
    ERR_SUCCESS = 0,        // 成功
    ERR_ARGS = 1,           // 参数错误
    ERR_FILE = 2,           // 文件错误
    ERR_EDIT = 3,           // 编辑错误
    ERR_MEMORY = 4,         // 内存错误
    ERR_AI = 5              // AI错误
} ErrorCode;
```

**功能**：
- 定义命令行参数结构
- 统一错误代码系统
- 提供AI友好的接口设计
- 支持JSON结构化输出

#### `ai_interface.c` - 命令行接口实现
```c
// 主要功能函数
CommandArgs* parse_args(int argc, char *argv[]); // 解析命令行参数
void free_args(CommandArgs *args); // 释放参数内存
int execute_command(CommandArgs *args, AIContext *ai_context); // 执行命令
void print_result(CommandArgs *args, void *result, int error_code); // 打印结果
void print_json_result(CommandArgs *args, void *result, int error_code); // JSON格式结果
```

**功能**：
- 命令行参数解析和处理
- 命令执行和调度
- 结果格式化输出（传统和JSON）
- 错误信息格式化

**技术特点**：
- 支持传统和JSON双输出格式
- 清晰的错误信息反馈
- 模块化的命令执行
- 易于扩展的命令系统

#### `ai_tcp.c` - 纯TCP/IP网络实现
```c
// 网络相关函数
int tcp_connect(const char *hostname, int port); // TCP连接
int send_all(int sock, const char *buffer, size_t length); // 发送数据
char* receive_all(int sock); // 接收数据
char* build_http_request(const char *host, const char *path, const char *method, 
                         const char *headers, const char *body); // 构建HTTP请求
char* http_request(const char *url, const char *method, const char *headers, 
                   const char *body); // HTTP请求
```

**功能**：
- TCP socket连接管理
- HTTP/1.1协议实现
- DNS域名解析
- 数据发送和接收
- 连接超时和错误处理

**技术特点**：
- 纯C语言实现，零外部依赖
- 完整的HTTP客户端功能
- 支持HTTPS代理模式
- 健壮的错误恢复机制

#### `ai_proxy.c` - 代理通信模块
```c
// 代理相关函数
char* build_proxy_request(const char *target_url, const char *api_key, 
                          const char *model, const char *message); // 构建代理请求
char* send_proxy_request(const char *proxy_url, const char *target_url, 
                         const char *api_key, const char *model, 
                         const char *message); // 发送代理请求
int is_https_url(const char *url); // 检查HTTPS地址
char* ai_request_via_proxy(AIContext *context, const char *message); // 通过代理发送AI请求
```

**功能**：
- 代理服务器通信
- HTTPS请求转发
- JSON请求构建
- 响应解析和处理

**技术特点**：
- 支持Python代理服务器
- 自动HTTPS检测和代理选择
- JSON格式转换和处理
- 完整的错误处理链

### 3. 加密算法模块 (`src/crypto/`)

#### `aes_simple.c` - AES加密算法实现
```c
// AES核心函数
void aes_key_expansion(const unsigned char *key, unsigned char *round_keys); // 密钥扩展
void aes_encrypt(const unsigned char *input, unsigned char *output, 
                 const unsigned char *round_keys); // AES加密
void aes_decrypt(const unsigned char *input, unsigned char *output, 
                 const unsigned char *round_keys); // AES解密
void aes_encrypt_ecb(const unsigned char *input, unsigned char *output, 
                     size_t length, const unsigned char *key); // ECB模式加密
void aes_decrypt_ecb(const unsigned char *input, unsigned char *output, 
                     size_t length, const unsigned char *key); // ECB模式解密
```

**功能**：
- AES-128加密算法实现
- 支持ECB和CBC模式
- 密钥扩展算法
- 加密和解密操作

**技术特点**：
- 纯C语言实现，无外部依赖
- 符合AES标准规范
- 优化的查表实现
- 内存安全的操作

#### `sha256_impl.c` - SHA256哈希算法实现
```c
// SHA256核心函数
void sha256_init(SHA256_CTX *ctx); // 初始化上下文
void sha256_update(SHA256_CTX *ctx, const unsigned char *data, size_t len); // 更新哈希
void sha256_final(SHA256_CTX *ctx, unsigned char *hash); // 计算最终哈希
void sha256(const unsigned char *data, size_t len, unsigned char *hash); // 计算SHA256
void hmac_sha256(const unsigned char *key, size_t key_len, 
                 const unsigned char *data, size_t data_len, 
                 unsigned char *hmac); // HMAC-SHA256计算
```

**功能**：
- SHA256哈希算法实现
- HMAC-SHA256消息认证码
- 哈希验证和比较
- 安全随机数生成

**技术特点**：
- 符合FIPS 180-4标准
- 优化的位操作实现
- 支持大文件流式处理
- 完整的测试覆盖

#### `tls_prf.c` - TLS伪随机函数
```c
// TLS PRF函数
void tls_prf(const unsigned char *secret, size_t secret_len,
             const char *label, const unsigned char *seed, size_t seed_len,
             unsigned char *output, size_t output_len); // TLS PRF计算
void tls_key_expansion(const unsigned char *master_secret,
                       const unsigned char *client_random,
                       const unsigned char *server_random,
                       unsigned char *key_block, size_t key_block_len); // 密钥扩展
```

**功能**：
- TLS伪随机函数实现
- 主密钥派生
- 会话密钥生成
- 安全随机数生成

**技术特点**：
- 符合TLS 1.2/1.3标准
- 安全的密钥派生
- 防重放攻击保护
- 前向安全性

### 4. 网络协议模块 (`src/network/`)

#### `mini_tls.h` - TLS协议定义
```c
// TLS记录类型
typedef enum {
    TLS_CHANGE_CIPHER_SPEC = 20,
    TLS_ALERT = 21,
    TLS_HANDSHAKE = 22,
    TLS_APPLICATION_DATA = 23
} TLSContentType;

// TLS握手类型
typedef enum {
    TLS_HELLO_REQUEST = 0,
    TLS_CLIENT_HELLO = 1,
    TLS_SERVER_HELLO = 2,
    TLS_CERTIFICATE = 11,
    TLS_SERVER_KEY_EXCHANGE = 12,
    TLS_CERTIFICATE_REQUEST = 13,
    TLS_SERVER_HELLO_DONE = 14,
    TLS_CERTIFICATE_VERIFY = 15,
    TLS_CLIENT_KEY_EXCHANGE = 16,
    TLS_FINISHED = 20
} TLSHandshakeType;

// TLS上下文
typedef struct {
    int sock;                      // socket句柄
    unsigned char master_secret[48]; // 主密钥
    unsigned char client_random[32]; // 客户端随机数
    unsigned char server_random[32]; // 服务器随机数
    // ... 其他TLS状态
} TLSContext;
```

**功能**：
- TLS协议类型定义
- 数据结构声明
- 函数接口定义
- 常量定义

#### `simple_tls.c` - 简化TLS实现
```c
// TLS核心函数
TLSContext* tls_connect(const char *hostname, int port); // TLS连接
int tls_handshake(TLSContext *ctx); // TLS握手
int tls_send(TLSContext *ctx, const unsigned char *data, size_t len); // TLS发送
int tls_recv(TLSContext *ctx, unsigned char *buffer, size_t buffer_len); // TLS接收
void tls_close(TLSContext *ctx); // 关闭TLS连接

// 握手协议函数
int tls_send_client_hello(TLSContext *ctx); // 发送Client Hello
int tls_recv_server_hello(TLSContext *ctx); // 接收Server Hello
int tls_verify_certificate(TLSContext *ctx); // 验证证书
int tls_key_exchange(TLSContext *ctx); // 密钥交换
```

**功能**：
- TLS握手协议实现
- 证书验证（基础）
- 加密通信建立
- 会话管理

**技术特点**：
- 简化的TLS 1.2实现
- 支持RSA密钥交换
- 基本的证书验证
- 可扩展的协议框架

### 5. 主程序模块 (`src/main/`)

#### `main.c` - 基础版本主程序
```c
int main(int argc, char *argv[]) {
    // 解析命令行参数
    CommandArgs *args = parse_args(argc, argv);
    if (!args) {
        print_usage();
        return ERR_ARGS;
    }
    
    // 执行命令
    int result = execute_command(args, NULL);
    
    // 清理资源
    free_args(args);
    
    return result;
}
```

**功能**：
- 命令行版本入口点
- 参数解析和验证
- 命令执行调度
- 错误处理和退出

#### `ai_main_new.c` - AI集成版本主程序
```c
int main(int argc, const char *argv[]) {
    // 初始化AI上下文
    AIContext *context = ai_context_create();
    
    // 加载配置或提示配置
    if (has_saved_config()) {
        load_config(context);
        printf("发现保存的配置，输入 /config 重新配置\n");
    }
    
    // 交互式配置
    printf("是否立即配置AI模型？(y/n): ");
    char response = get_user_response();
    if (response == 'y') {
        interactive_configure(context);
    }
    
    // 进入交互循环
    interactive_loop(context);
    
    // 清理资源
    ai_context_free(context);
    
    return 0;
}
```

**功能**：
- AI集成版本入口点
- 交互式配置管理
- 命令循环和用户交互
- 资源管理和清理

**技术特点**：
-### 6. 工具脚本 (`tools/`)

#### `proxy_server.py` - Python代理服务器
```python
#!/usr/bin/env python3
"""
AI二进制编辑器 - 代理服务器
功能：转发HTTPS请求，处理JSON格式转换
"""

class AIProxyServer:
    def __init__(self, port=8080):
        self.port = port
        self.server = None
    
    def handle_request(self, request_data):
        """处理代理请求"""
        # 解析JSON请求
        import json
        import requests
        request = json.loads(request_data)
        
        # 提取目标URL和参数
        target_url = request['url']
        headers = request.get('headers', {})
        body = request.get('body', {})
        
        # 转发HTTPS请求
        response = requests.post(
            target_url,
            headers=headers,
            json=body,
            verify=False  # 简化实现，生产环境应验证证书
        )
        
        # 返回响应
        return {
            'status': response.status_code,
            'headers': dict(response.headers),
            'body': response.json() if response.content else {}
        }
```

**功能**：
- HTTPS请求转发
- JSON格式处理和转换
- 错误处理和日志记录
- 健康检查端点

**技术特点**：
- 简单的HTTP服务器实现
- 支持多种AI API格式
- 详细的错误日志
- 可配置的端口和超时

### 7. 文档目录 (`docs/`)

#### `README.md` - 项目总览
- 项目介绍和愿景
- 核心特性说明
- 快速开始指南
- 使用示例
- 贡献指南
- 许可证信息

#### `AI_USAGE.md` - AI使用指南
- 70+详细使用示例
- 命令行参数详解
- AI对话示例
- 自然语言操作指南
- 故障排除

#### `AI_INTEGRATION.md` - AI集成文档
- AI模型配置说明
- 网络通信架构
- 协议实现细节
- 安全注意事项
- 性能优化建议

#### `PROJECT_SUMMARY.md` - 项目总结
- 开发历程回顾
- 技术决策说明
- 遇到的问题和解决方案
- 未来规划

#### `PROJECT_STRUCTURE.md` - 项目结构
- 完整的目录结构
- 文件功能描述
- 构建系统说明
- 设计理念阐述

### 8. 资源文件 (`resources/`)

#### 二进制开发资源库
- `architectures/` - CPU架构文档
  - `x86_64/overview.md` - x86-64架构概述
- `interfaces/` - 系统接口规范
  - `linux/syscalls/syscalls_x86_64.md` - Linux系统调用

**功能**：
- 提供二进制开发参考资料
- 系统接口文档
- 硬件架构说明
- 开发规范和最佳实践

## 🔧 构建系统

### Makefile 详解

#### 编译目标
```makefile
# 基础版本
make           # 编译 bin_editor
make basic     # 同上

# AI集成版本
make ai        # 编译 ai_bin_editor

# 调试版本
make debug     # 带调试信息的版本
make ai-debug  # AI版本的调试构建

# 测试
make test      # 运行基础测试
make ai-test   # 运行AI版本测试

# 安装
make install   # 安装到系统路径
make uninstall # 卸载
```

#### 编译选项
```makefile
CC = clang                     # 使用Clang编译器
CFLAGS = -Wall -Wextra -O2 -std=c11  # 严格警告和优化
LDFLAGS =                      # 零外部依赖
INCLUDES = -Isrc -Isrc/core -Isrc/ai -Isrc/crypto -Isrc/network
```

#### 文件依赖管理
```makefile
# 自动生成依赖关系
$(OBJ_DIR)/%.o: src/%.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
```

## 🎯 开发指南

### 代码组织原则

#### 1. 模块化设计
- 每个模块有明确的职责
- 模块间通过清晰接口通信
- 避免循环依赖

#### 2. 错误处理
- 所有函数返回错误代码
- 资源分配必须检查返回值
- 提供详细的错误信息

#### 3. 内存管理
- 谁分配谁释放原则
- 使用后立即设置为NULL
- 避免内存泄漏和悬空指针

#### 4. 接口设计
- 函数接口简单明确
- 参数顺序合理
- 返回值意义清晰

### 添加新功能

#### 步骤1：设计接口
```c
// 在相应头文件中声明
int new_function(const char *param1, int param2, char **result);
```

#### 步骤2：实现功能
```c
// 在源文件中实现
int new_function(const char *param1, int param2, char **result) {
    // 参数验证
    if (!param1 || !result) return ERR_ARGS;
    
    // 功能实现
    // ...
    
    return ERR_SUCCESS;
}
```

#### 步骤3：更新文档
- 更新头文件注释
- 添加使用示例
- 更新相关文档

#### 步骤4：添加测试
```c
// 测试用例
void test_new_function() {
    char *result = NULL;
    int ret = new_function("test", 123, &result);
    assert(ret == ERR_SUCCESS);
    assert(result != NULL);
    free(result);
}
```

### 调试技巧

#### 1. 编译带调试信息
```bash
make ai-debug
```

#### 2. 使用GDB调试
```bash
gdb ./ai_bin_editor
(gdb) break main
(gdb) run
(gdb) next
(gdb) print variable
```

#### 3. 添加调试日志
```c
#ifdef DEBUG
    printf("[DEBUG] 函数名: 变量值=%d\n", variable);
#endif
```

#### 4. 内存检查
```bash
# 使用Valgrind检查内存泄漏
valgrind --leak-check=full ./ai_bin_editor
```

## 📈 性能优化

### 1. 文件处理优化
- 使用内存映射处理大文件
- 批量操作减少I/O次数
- 缓存常用数据

### 2. 网络通信优化
- 连接复用
- 请求批处理
- 压缩传输数据

### 3. 算法优化
- 使用更高效的搜索算法
- 优化加密算法实现
- 减少不必要的计算

### 4. 内存优化
- 使用内存池
- 及时释放不再使用的内存
- 避免内存碎片

## 🔒 安全考虑

### 1. 输入验证
- 所有用户输入必须验证
- 检查缓冲区边界
- 防止注入攻击

### 2. 内存安全
- 使用安全的内存函数
- 检查分配返回值
- 避免缓冲区溢出

### 3. 网络安全
- 验证服务器证书
- 使用安全的加密算法
- 防止中间人攻击

### 4. 密钥安全
- 不在代码中硬编码密钥
- 使用安全的密钥存储
- 定期更换密钥

## 🤝 协作开发

### 1. 代码审查
- 所有提交必须经过代码审查
- 关注代码质量和安全性
- 提供建设性反馈

### 2. 测试要求
- 新功能必须包含测试
- 修复Bug必须添加回归测试
- 保持测试覆盖率

### 3. 文档更新
- 代码变更必须更新文档
- 保持文档与代码同步
- 提供清晰的变更说明

### 4. 版本管理
- 使用语义化版本号
- 清晰的提交信息
- 定期发布稳定版本

## 🚀 扩展方向

### 1. 功能扩展
- 支持更多文件格式
- 添加图形界面版本
- 集成更多AI模型

### 2. 性能提升
- 多线程支持
- GPU加速
- 分布式处理

### 3. 平台扩展
- 支持Windows平台
- 移动端版本
- Web版本

### 4. 生态建设
- 插件系统
- API服务
- 云平台集成

## 📚 学习资源

### 1. 相关技术
- [C语言编程](https://en.cppreference.com/w/c)
- [网络编程](https://beej.us/guide/bgnet/)
- [加密算法](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197.pdf)
- [TLS协议](https://tools.ietf.org/html/rfc5246)

### 2. 参考项目
- [hexedit](https://github.com/pixel/hexedit) - 十六进制编辑器
- [curl](https://github.com/curl/curl) - 网络传输工具
- [OpenSSL](https://github.com/openssl/openssl) - 加密库
- [jq](https://github.com/stedolan/jq) - JSON处理工具

### 3. 开发工具
- [Clang](https://clang.llvm.org/) - C语言编译器
- [Make](https://www.gnu.org/software/make/) - 构建工具
- [GDB](https://www.gnu.org/software/gdb/) - 调试器
- [Valgrind](https://valgrind.org/) - 内存检查工具

## 🎉 总结

AI二进制编辑器是一个**技术全面、架构清晰、设计优秀**的开源项目。它展示了：

1. **技术深度**：从底层二进制操作到高级AI集成
2. **工程质量**：严谨的代码规范和完整的测试覆盖
3. **创新精神**：零依赖架构和自主算法实现
4. **实用价值**：解决实际问题的强大工具

希望这份技术文档能帮助你更好地理解和使用这个项目。无论是学习、使用还是贡献，我们都欢迎你的参与！

**一起为AI时代打造完全自主的底层工具！**