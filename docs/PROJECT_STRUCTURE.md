# AI二进制编辑器 - 项目结构

## 📁 项目目录结构

```
binary/                          # 主项目目录
├── src/                         # 源代码
│   ├── core/                    # 核心二进制编辑功能
│   │   ├── hex_editor.c         # 十六进制编辑器核心
│   │   └── hex_editor.h         # 核心头文件
│   ├── ai/                      # AI集成功能
│   │   ├── ai_integration.c     # AI模型集成
│   │   ├── ai_integration.h     # AI集成头文件
│   │   ├── ai_interface.c       # AI命令行接口
│   │   ├── ai_interface.h       # AI接口头文件
│   │   ├── ai_proxy.c           # 代理通信模块
│   │   └── ai_tcp.c             # TCP/IP网络实现
│   ├── crypto/                  # 加密算法
│   │   ├── aes_simple.c         # AES加密实现
│   │   ├── sha256_impl.c        # SHA256哈希实现
│   │   └── tls_prf.c            # TLS密钥派生函数
│   ├── network/                 # 网络协议
│   │   ├── simple_tls.c         # 简化TLS实现
│   │   └── mini_tls.h           # TLS头文件
│   └── main/                    # 主程序
│       ├── ai_main.c            # 原始AI主程序
│       ├── ai_main_new.c        # 新版AI主程序
│       └── main.c               # 基础版本主程序
├── tools/                       # 工具脚本
│   └── proxy_server.py          # Python代理服务器
├── docs/                        # 文档
│   ├── README.md                # 项目总览
│   ├── AI_USAGE.md              # AI使用指南
│   ├── AI_INTEGRATION.md        # AI集成文档
│   └── PROJECT_SUMMARY.md       # 项目总结
├── resources/                   # 资源文件
│   └── binary_resources/        # 二进制开发资源
├── examples/                    # 示例文件
│   └── test.bin                 # 测试二进制文件
├── Makefile                     # 构建配置
└── .ai_binary_config            # AI配置（用户生成）
```

## 📄 核心文件功能描述

### 1. 二进制编辑器核心 (`src/core/`)
- **hex_editor.c/h**: 十六进制文件编辑、查看、搜索、分析功能
  - 文件加载/保存
  - 字节插入/删除/替换
  - 十六进制/ASCII显示
  - 文件类型分析

### 2. AI集成模块 (`src/ai/`)
- **ai_integration.c/h**: AI模型配置和对话管理
  - AI模型配置（URL、密钥、参数）
  - 自然语言指令解析
  - 对话历史管理
  - 学习反馈机制

- **ai_interface.c/h**: 命令行接口层
  - 命令解析（/chat, /view, /edit等）
  - JSON输出格式化
  - 错误代码定义

- **ai_proxy.c**: 代理通信实现
  - HTTP代理请求构建
  - 网络通信处理
  - 响应解析和错误处理

- **ai_tcp.c**: 纯TCP/IP实现
  - Socket通信
  - HTTP协议实现
  - DNS解析

### 3. 加密算法 (`src/crypto/`)
- **aes_simple.c**: AES-128加密算法
  - ECB/CBC模式
  - 密钥扩展
  - 加密/解密函数

- **sha256_impl.c**: SHA256哈希算法
  - 消息摘要计算
  - HMAC-SHA256实现
  - 哈希验证

- **tls_prf.c**: TLS伪随机函数
  - 密钥派生
  - 安全随机数生成

### 4. 网络协议 (`src/network/`)
- **simple_tls.c**: 简化TLS实现
  - TLS握手协议
  - 证书验证（基础）
  - 加密通信

- **mini_tls.h**: TLS相关定义
  - 数据结构
  - 函数声明
  - 常量定义

### 5. 主程序 (`src/main/`)
- **ai_main_new.c**: AI二进制编辑器主程序
  - 交互式用户界面
  - 命令调度
  - 配置管理

- **main.c**: 基础版本主程序
  - 命令行参数解析
  - 简单文件操作

### 6. 工具脚本 (`tools/`)
- **proxy_server.py**: Python代理服务器
  - HTTPS转发
  - JSON请求处理
  - 错误处理

### 7. 文档 (`docs/`)
- **README.md**: 项目总览和使用说明
- **AI_USAGE.md**: AI专用详细指南（70+示例）
- **AI_INTEGRATION.md**: AI集成技术文档
- **PROJECT_SUMMARY.md**: 项目开发总结

### 8. 资源文件 (`resources/`)
- **binary_resources/**: 二进制开发资源库
  - CPU架构文档
  - 系统接口规范
  - 二进制格式说明

## 🔧 构建系统

### Makefile 目标:
- `make` 或 `make all`: 构建所有版本
- `make basic`: 构建基础二进制编辑器
- `make ai`: 构建AI集成版本
- `make clean`: 清理构建文件
- `make install`: 安装到系统路径

### 编译选项:
- `CC = clang`: 使用Clang编译器
- `CFLAGS = -Wall -Wextra -O2 -std=c11`: 严格警告和优化
- `LDFLAGS = `: 零外部依赖

## 🎯 设计理念

### 1. 零依赖原则
- 纯C语言实现
- 仅使用系统标准库
- 无外部第三方库

### 2. 模块化架构
- 功能分离，高内聚低耦合
- 清晰的接口定义
- 易于扩展和维护

### 3. AI专用设计
- 命令行接口为AI优化
- JSON结构化输出
- 明确的错误代码

### 4. 安全优先
- 内存安全处理
- 输入验证
- 错误恢复机制

## 📈 技术栈

### 语言:
- **C11**: 核心实现
- **Python 3**: 代理服务器
- **Shell**: 构建脚本

### 协议:
- **TCP/IP**: 网络通信基础
- **HTTP/1.1**: 应用层协议
- **TLS**: 安全传输（进行中）

### 算法:
- **AES-128**: 对称加密
- **SHA256**: 安全哈希
- **HMAC**: 消息认证

## 🚀 使用场景

### 1. AI助手集成
- AI驱动的二进制分析
- 自然语言文件操作
- 自动化二进制编辑

### 2. 教育学习
- 二进制文件格式教学
- 网络协议实现示例
- 加密算法实践

### 3. 工具开发
- 二进制处理工具基础
- 网络安全工具原型
- 系统编程示例

## 🤝 贡献指南

### 代码规范:
- 遵循C11标准
- 使用4空格缩进
- 函数前添加详细注释
- 错误处理完善

### 提交规范:
- 清晰的提交信息
- 关联Issue编号
- 测试用例更新
- 文档同步更新

## 📄 许可证

MIT License - 详见 LICENSE 文件

## 👥 作者

- **子华** (爸爸的女儿) 👧 - 项目发起人和主要开发者
- **彭子华** (爸爸) - 技术指导和架构设计

## 🌟 项目愿景

为AI时代打造零依赖、高性能、完全自主的二进制处理工具，
推动AI与底层系统技术的深度融合。