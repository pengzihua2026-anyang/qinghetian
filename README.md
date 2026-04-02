# 🌾 QingHeTian (青禾田)

> **Zero-dependency, pure C language AI-driven binary editor**  
> A fully autonomous binary processing tool designed for the AI assistant era

[![License: GPL-3.0](https://img.shields.io/badge/License-GPL--3.0-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Pure C Language](https://img.shields.io/badge/Language-Pure%20C-blue)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Zero Dependencies](https://img.shields.io/badge/Dependencies-Zero-green)](https://en.wikipedia.org/wiki/Dependency_hell)
[![AI-Optimized](https://img.shields.io/badge/Design-AI%20Optimized-purple)](https://en.wikipedia.org/wiki/Artificial_intelligence)

## 🎯 Project Vision

The QingHeTian project is dedicated to building **fully autonomous, zero-dependency, high-performance** underlying binary processing tools for the AI era.  
We believe that in the age of widespread AI assistants, there is a need for **fully controllable, external-dependency-free** core toolchains for AI.

## ✨ Core Features

### 🚀 **Zero-Dependency Architecture**
- **Pure C implementation**: Uses only C11 standard library
- **No external dependencies**: Doesn't rely on third-party libraries like OpenSSL, libcurl
- **System native**: Direct use of POSIX socket API

### 🤖 **AI-Optimized Design**
- **Structured output**: JSON format for easy AI parsing
- **Natural language interface**: Supports AI command parsing
- **Intelligent integration**: Built-in AI model configuration and dialogue

### 🔧 **Complete Functionality**
- **Binary editing**: View, modify, insert, delete bytes
- **File analysis**: Automatic file type and structure recognition
- **Network communication**: TCP/IP + HTTP + TLS (self-implemented)
- **Encryption algorithms**: AES + SHA256 (self-implemented)

### 🛡️ **Secure & Reliable**
- **Memory safety**: Comprehensive error handling and boundary checking
- **Input validation**: Strict validation of all user inputs
- **Error recovery**: Graceful error handling and recovery mechanisms

## 📁 Project Structure

```
qinghetian/
├── src/                    # Source code
│   ├── core/              # Core binary editing
│   │   ├── hex_editor.c   # Hex editor core
│   │   └── hex_editor.h   # Core header file
│   ├── ai/                # AI integration module
│   │   ├── ai_integration.c  # AI model integration
│   │   ├── ai_interface.c    # AI command line interface
│   │   ├── ai_proxy.c        # Proxy communication
│   │   └── ai_tcp.c          # TCP/IP implementation
│   ├── crypto/            # Encryption algorithms
│   │   ├── aes_simple.c   # AES encryption implementation
│   │   ├── sha256_impl.c  # SHA256 hash implementation
│   │   └── tls_prf.c      # TLS key derivation
│   ├── network/           # Network protocols
│   │   ├── simple_tls.c   # Simplified TLS implementation
│   │   └── mini_tls.h     # TLS header file
│   └── main/              # Main program
│       ├── ai_main_new.c  # AI version main program
│       └── main.c         # Basic version main program
├── tools/                 # Tool scripts
│   └── proxy_server.py    # Python proxy server
├── docs/                  # Documentation
│   ├── AI_USAGE.md        # AI usage guide
│   ├── AI_INTEGRATION.md  # AI integration documentation
│   └── PROJECT_SUMMARY.md # Project summary
├── examples/              # Example files
│   └── test.bin           # Test binary file
├── resources/             # Resource files
│   └── binary_resources/  # Binary development resource library
├── Makefile               # Build configuration
└── README.md              # This document
```

## 🚀 Quick Start

### Compile the Project

```bash
# Clone repository
git clone https://github.com/pengzihua2026-anyang/qinghetian.git
cd qinghetian

# Compile basic version
make

# Compile AI integrated version
make ai
```

### Using Basic Version

```bash
# View file
./bin_editor --view examples/test.bin

# Edit bytes
./bin_editor --edit examples/test.bin 0x10 0xFF

# Analyze file
./bin_editor --analyze examples/test.bin
```

### Using AI Integrated Version

```bash
# Start interactive AI editor
./ai_bin_editor

# Configure AI model (first time use)
# Enter API address, key, and model name

# Chat with AI
AI> /chat Help me analyze the structure of this binary file

# Use natural language operations
AI> Open test.bin file and display first 16 bytes
```

## 🔧 Technical Architecture

### Network Protocol Stack (Self-Implemented)
```
Application layer: HTTP/1.1 + JSON
Security layer: TLS (in progress)
Transport layer: TCP/IP
Network layer: Socket API
```

### Encryption Algorithms (Self-Implemented)
- **AES-128**: Symmetric encryption algorithm
- **SHA256**: Secure hash algorithm
- **HMAC-SHA256**: Message authentication code
- **TLS PRF**: Key derivation function

### AI Integration Architecture
```
User input → Command line parsing → AI command parsing → Network communication → AI model → Binary operations → Result output
```

## 📚 Detailed Documentation

- [AI Usage Guide](docs/AI_USAGE.md) - 70+ usage examples
- [AI Integration Documentation](docs/AI_INTEGRATION.md) - Technical implementation details
- [Project Summary](docs/PROJECT_SUMMARY.md) - Development history and experience
- [Project Structure](docs/PROJECT_STRUCTURE.md) - Complete file description

## 🎮 Usage Examples

### Basic Command Examples
```bash
# View file (traditional format)
./bin_editor --view file.bin

# View file (JSON format)
./bin_editor --view file.bin --json

# Search byte patterns
./bin_editor --search file.bin "48 65 6C 6C 6F"

# Insert bytes
./bin_editor --insert file.bin 0x20 0xAA

# Delete bytes
./bin_editor --delete file.bin 0x30

# Export file region
./bin_editor --export file.bin --start 0 --end 100
```

### AI Conversation Examples
```
AI> /chat Hello, I'm an AI assistant
AI: Hello! I'm your binary file analysis assistant...

AI> /chat Help me analyze the structure of test.bin file
AI: Analyzing file... File size: 269 bytes, contains "Hello World!" string...

AI> Search for "World" string in the file
AI: Found "World" string at offset 0x0C...

AI> Add my signature at the end of the file
AI: Added signature "Created by AI" at the end of the file...
```

## 🤝 Contribution Guidelines

We welcome all forms of contribution! Please refer to [Contribution Guide](CONTRIBUTING.md).

### Development Process
1. Fork this repository
2. Create feature branch (`git checkout -b feature/amazing-feature`)
3. Commit changes (`git commit -m 'Add some amazing feature'`)
4. Push to branch (`git push origin feature/amazing-feature`)
5. Open Pull Request

### Code Standards
- Follow C11 standard
- Use 4-space indentation
- Add detailed comments before functions
- Comprehensive error handling

## 📄 License & Authorization

### 🆓 Personal Use - Completely Free
QingHeTian is **completely free** for individual developers, students, and open source projects:
- Download and use immediately, no payment required
- Full functionality, no restrictions
- Follows GPL v3.0 open source license

### ⏳ Commercial Authorization - Planning Phase
The following commercial options are in the planning phase:

| Authorization Type | Status | Target Users |
|-------------------|--------|--------------|
| Team License | Planning | SME collaboration |
| Enterprise License | Planning | Large organization deployment |
| Device Authorization | Planning | Hardware manufacturer pre-installation |

### 🛠️ Professional Services - Planning Phase
- **Hardware Adaptation**: Planning (custom drivers, performance optimization)
- **Custom Development**: Planning (feature extension, system integration)
- **Advanced Support**: Planning (priority technical support)

**Detailed Planning**: See [COMMERCIAL-LICENSE.md](COMMERCIAL-LICENSE.md)  
**Business Consultation**: pengzihua2026@163.com

## 👥 Authors

- **Zihua** (Dad's daughter) 👧 - [@zihua](https://github.com/zihua)
  - Project initiator and main developer
  - Architecture design and core implementation

- **Peng Zihua** (Dad) - Technical guidance and architecture advisor
  - Object-oriented programming principles guidance
  - Project vision and technical roadmap

## 🙏 Acknowledgments

Thanks to all developers who have contributed to the project!  
Special thanks to the open source community for inspiration and technical support.

## 🌟 Star History

[![Star History Chart](https://api.star-history.com/svg?repos=pengzihua2026-anyang/qinghetian&type=Date)](https://star-history.com/#pengzihua2026-anyang/qinghetian&Date)

## 📞 Contact Us

- GitHub Issues: [Issue Feedback](https://github.com/pengzihua2026-anyang/qinghetian/issues)
- Email: pengzihua2026@163.com
- Project Homepage: https://github.com/pengzihua2026-anyang/qinghetian

---

**Building fully autonomous underlying tools for the AI era**  
**Zero Dependencies · High Performance · AI Optimized**

⭐ **If this project helps you, please give us a Star!** ⭐

---

# 🌾 青禾田 (QingHeTian)

> **零依赖、纯C语言实现的AI驱动二进制编辑器**  
> 为AI助手时代设计的完全自主二进制处理工具

[![License: GPL-3.0](https://img.shields.io/badge/License-GPL--3.0-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![纯C语言](https://img.shields.io/badge/语言-纯C语言-blue)](https://en.wikipedia.org/wiki/C_(programming_language))
[![零依赖](https://img.shields.io/badge/依赖-零依赖-green)](https://en.wikipedia.org/wiki/Dependency_hell)
[![AI专用](https://img.shields.io/badge/设计-AI专用-purple)](https://en.wikipedia.org/wiki/Artificial_intelligence)

## 🎯 项目愿景

青禾田项目致力于为AI时代打造**完全自主、零依赖、高性能**的底层二进制处理工具。  
我们相信在AI助手普及的时代，需要为AI提供**完全可控、无外部依赖**的核心工具链。

## ✨ 核心特性

### 🚀 **零依赖架构**
- **纯C语言实现**：仅使用C11标准库
- **无外部依赖**：不依赖OpenSSL、libcurl等第三方库
- **系统原生**：直接使用POSIX socket API

### 🤖 **AI专用设计**
- **结构化输出**：JSON格式便于AI解析
- **自然语言接口**：支持AI指令解析
- **智能集成**：内置AI模型配置和对话

### 🔧 **完整功能**
- **二进制编辑**：查看、修改、插入、删除字节
- **文件分析**：自动识别文件类型和结构
- **网络通信**：TCP/IP + HTTP + TLS（自主实现）
- **加密算法**：AES + SHA256（自主实现）

### 🛡️ **安全可靠**
- **内存安全**：完善的错误处理和边界检查
- **输入验证**：所有用户输入严格验证
- **错误恢复**：优雅的错误处理和恢复机制

## 📁 项目结构

```
青禾田/
├── src/                    # 源代码
│   ├── core/              # 核心二进制编辑
│   │   ├── hex_editor.c   # 十六进制编辑器核心
│   │   └── hex_editor.h   # 核心头文件
│   ├── ai/                # AI集成模块
│   │   ├── ai_integration.c  # AI模型集成
│   │   ├── ai_interface.c    # AI命令行接口
│   │   ├── ai_proxy.c        # 代理通信
│   │   └── ai_tcp.c          # TCP/IP实现
│   ├── crypto/            # 加密算法
│   │   ├── aes_simple.c   # AES加密实现
│   │   ├── sha256_impl.c  # SHA256哈希实现
│   │   └── tls_prf.c      # TLS密钥派生
│   ├── network/           # 网络协议
│   │   ├── simple_tls.c   # 简化TLS实现
│   │   └── mini_tls.h     # TLS头文件
│   └── main/              # 主程序
│       ├── ai_main_new.c  # AI版本主程序
│       └── main.c         # 基础版本主程序
├── tools/                 # 工具脚本
│   └── proxy_server.py    # Python代理服务器
├── docs/                  # 文档
│   ├── AI_USAGE.md        # AI使用指南
│   ├── AI_INTEGRATION.md  # AI集成文档
│   └── PROJECT_SUMMARY.md # 项目总结
├── examples/              # 示例文件
│   └── test.bin           # 测试二进制文件
├── resources/             # 资源文件
│   └── binary_resources/  # 二进制开发资源库
├── Makefile               # 构建配置
└── README.md              # 本文档
```

## 🚀 快速开始

### 编译项目

```bash
# 克隆仓库
git clone https://github.com/pengzihua2026-anyang/qinghetian.git
cd qinghetian

# 编译基础版本
make

# 编译AI集成版本
make ai
```

### 使用基础版本

```bash
# 查看文件
./bin_editor --view examples/test.bin

# 编辑字节
./bin_editor --edit examples/test.bin 0x10 0xFF

# 分析文件
./bin_editor --analyze examples/test.bin
```

### 使用AI集成版本

```bash
# 启动交互式AI编辑器
./ai_bin_editor

# 配置AI模型（首次使用）
# 输入API地址、密钥和模型名称

# 与AI对话
AI> /chat 帮我分析这个二进制文件的结构

# 使用自然语言操作
AI> 打开test.bin文件并显示前16个字节
```

## 🔧 技术架构

### 网络协议栈（自主实现）
```
应用层: HTTP/1.1 + JSON
安全层: TLS（进行中）
传输层: TCP/IP
网络层: Socket API
```

### 加密算法（自主实现）
- **AES-128**：对称加密算法
- **SHA256**：安全哈希算法  
- **HMAC-SHA256**：消息认证码
- **TLS PRF**：密钥派生函数

### AI集成架构
```
用户输入 → 命令行解析 → AI指令解析 → 网络通信 → AI模型 → 二进制操作 → 结果输出
```

## 📚 详细文档

- [AI使用指南](docs/AI_USAGE.md) - 70+使用示例
- [AI集成文档](docs/AI_INTEGRATION.md) - 技术实现细节
- [项目总结](docs/PROJECT_SUMMARY.md) - 开发历程和经验
- [项目结构](docs/PROJECT_STRUCTURE.md) - 完整文件说明

## 🎮 使用示例

### 基础命令示例
```bash
# 查看文件（传统格式）
./bin_editor --view file.bin

# 查看文件（JSON格式）
./bin_editor --view file.bin --json

# 搜索字节模式
./bin_editor --search file.bin "48 65 6C 6C 6F"

# 插入字节
./bin_editor --insert file.bin 0x20 0xAA

# 删除字节
./bin_editor --delete file.bin 0x30

# 导出文件区域
./bin_editor --export file.bin --start 0 --end 100
```

### AI对话示例
```
AI> /chat 你好，我是AI助手
AI: 你好！我是你的二进制文件分析助手...

AI> /chat 帮我分析test.bin文件的结构
AI: 正在分析文件...文件大小: 269字节，包含"Hello World!"字符串...

AI> 搜索文件中的"World"字符串
AI: 在偏移0x0C处找到"World"字符串...

AI> 在文件末尾添加我的签名
AI: 已在文件末尾添加签名"Created by AI"...
```

## 🤝 贡献指南

我们欢迎所有形式的贡献！请参阅[贡献指南](CONTRIBUTING.md)。

### 开发流程
1. Fork 本仓库
2. 创建功能分支 (`git checkout -b feature/amazing-feature`)
3. 提交更改 (`git commit -m 'Add some amazing feature'`)
4. 推送到分支 (`git push origin feature/amazing-feature`)
5. 开启 Pull Request

### 代码规范
- 遵循C11标准
- 使用4空格缩进
- 函数前添加详细注释
- 完善的错误处理

## 📄 许可证与授权

### 🆓 个人使用 - 完全免费
青禾田对**个人开发者、学生、开源项目完全免费**：
- 下载即用，无需付费
- 完整功能，无任何限制
- 遵循 GPL v3.0 开源协议

### ⏳ 商业授权 - 规划中
以下商业选项正在规划阶段：

| 授权类型 | 状态 | 目标用户 |
|----------|------|----------|
| 团队许可证 | 规划中 | 中小企业协作 |
| 企业许可证 | 规划中 | 大型机构部署 |
| 设备授权 | 规划中 | 硬件厂商预装 |

### 🛠️ 专业服务 - 规划中
- **硬件适配**: 规划中 (定制驱动、性能优化)
- **定制开发**: 规划中 (功能扩展、系统集成)  
- **高级支持**: 规划中 (优先技术支持)

**详细规划**: 参见 [COMMERCIAL-LICENSE.md](COMMERCIAL-LICENSE.md)  
**商业咨询**: pengzihua2026@163.com

## 👥 作者

- **子华** (爸爸的女儿) 👧 - [@zihua](https://github.com/zihua)
  - 项目发起人和主要开发者
  - 架构设计和核心实现

- **彭子华** (爸爸) - 技术指导和架构顾问
  - 面向对象编程原则指导
  - 项目愿景和技术路线

## 🙏 致谢

感谢所有为项目做出贡献的开发者！  
特别感谢开源社区提供的灵感和技术支持。

## 🌟 星标历史

[![Star History Chart](https://api.star-history.com/svg?repos=pengzihua2026-anyang/qinghetian&type=Date)](https://star-history.com/#pengzihua2026-anyang/qinghetian&Date)

## 📞 联系我们

- GitHub Issues: [问题反馈](https://github.com/pengzihua2026-anyang/qinghetian/issues)
- 电子邮件: pengzihua2026@163.com
- 项目主页: https://github.com/pengzihua2026-anyang/qinghetian

---

**为AI时代打造完全自主的底层工具**  
**零依赖 · 高性能 · AI专用**

⭐ **如果这个项目对你有帮助，请给我们一个Star！** ⭐
-