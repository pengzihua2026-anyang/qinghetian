# 🤖 AI二进制编辑器

> **零依赖、纯C语言实现的AI驱动二进制编辑器**  
> 为AI助手时代设计的完全自主二进制处理工具

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![纯C语言](https://img.shields.io/badge/语言-纯C语言-blue)](https://en.wikipedia.org/wiki/C_(programming_language))
[![零依赖](https://img.shields.io/badge/依赖-零依赖-green)](https://en.wikipedia.org/wiki/Dependency_hell)
[![AI专用](https://img.shields.io/badge/设计-AI专用-purple)](https://en.wikipedia.org/wiki/Artificial_intelligence)

## 🎯 项目愿景

在AI时代，我们需要为AI助手打造**完全自主、零依赖、高性能**的底层工具。  
AI二进制编辑器正是为此而生——一个用纯C语言实现，不依赖任何外部库，专为AI优化的二进制处理工具。

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
ai-binary-editor/
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
git clone https://github.com/zihua/ai-binary-editor.git
cd ai-binary-editor

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

## 📄 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件。

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

[![Star History Chart](https://api.star-history.com/svg?repos=pengzihua2026-anyang/ai-binary-editor&type=Date)](https://star-history.com/#pengzihua2026-anyang/ai-binary-editor&Date)

## 📞 联系我们

- GitHub Issues: [问题反馈](https://github.com/pengzihua2026-anyang/ai-binary-editor/issues)
- 电子邮件: pengzihua2026@163.com
- 项目主页: https://github.com/pengzihua2026-anyang/ai-binary-editor

---

**为AI时代打造完全自主的底层工具**  
**零依赖 · 高性能 · AI专用**

⭐ **如果这个项目对你有帮助，请给我们一个Star！** ⭐