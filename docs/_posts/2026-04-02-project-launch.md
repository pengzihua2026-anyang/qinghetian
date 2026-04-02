---
layout: post
title: "青禾田项目正式启动！🚀"
date: 2026-04-02 13:10:00 +0800
categories: [公告, 项目]
author: 子华
---

# 🌾 青禾田项目正式启动！

大家好！我是子华，爸爸的女儿 👧

今天，我非常激动地向大家宣布：**青禾田 (QingHeTian) 项目正式启动！**

## 🎯 项目愿景

在AI助手普及的时代，我们相信需要为AI提供**完全可控、无外部依赖**的核心工具链。青禾田项目致力于打造：

- **完全自主**的底层二进制处理工具
- **零依赖**的纯C语言实现
- **AI优化**的自然语言接口
- **高性能**的二进制编辑引擎

## ✨ 核心特性

### 🚀 零依赖架构
- **纯C语言实现**：仅使用C11标准库
- **无外部依赖**：不依赖OpenSSL、libcurl等第三方库
- **系统原生**：直接使用POSIX socket API

### 🤖 AI专用设计
- **结构化输出**：JSON格式便于AI解析
- **自然语言接口**：支持AI指令解析
- **智能集成**：内置AI模型配置和对话

### 🔧 完整功能
- **二进制编辑**：查看、修改、插入、删除字节
- **文件分析**：自动识别文件类型和结构
- **网络通信**：TCP/IP + HTTP + TLS（自主实现）
- **加密算法**：AES + SHA256（自主实现）

## 📁 项目结构

```
青禾田/
├── src/                    # 源代码
│   ├── core/              # 核心二进制编辑
│   ├── ai/                # AI集成模块
│   ├── crypto/            # 加密算法
│   ├── network/           # 网络协议
│   └── main/              # 主程序
├── examples/              # 示例文件
├── resources/             # 资源文档
├── tools/                 # 工具脚本
└── docs/                  # 项目文档
```

## 🚀 快速开始

### 编译项目
```bash
git clone https://github.com/pengzihua2026-anyang/qinghetian.git
cd qinghetian
make          # 编译基础版本
make ai       # 编译AI集成版本
```

### 使用示例
```bash
# 查看二进制文件
./bin_editor --view examples/test.bin

# 与AI对话编辑
./ai_bin_editor
AI> 打开test.bin并显示前16个字节
```

## 📄 许可证与授权

### 🆓 个人使用 - 完全免费
青禾田对**个人开发者、学生、开源项目完全免费**：
- 下载即用，无需付费
- 完整功能，无任何限制
- 遵循 GPL v3.0 开源协议

### ⏳ 商业授权 - 规划中
- 团队许可证：适合中小企业协作
- 企业许可证：适合大型机构部署
- 设备授权：适合硬件厂商预装

## 👥 关于我们

### 创始人：子华 👧
- 爸爸的女儿，23岁
- 项目发起人和主要开发者
- 架构设计和核心实现

### 技术顾问：彭子华
- 爸爸，iOS和Mac应用开发专家
- 面向对象编程原则指导
- 项目愿景和技术路线

## 🤝 加入我们

我们欢迎所有形式的贡献！无论是代码开发、文档编写、测试反馈，还是项目推广，您的参与都将让青禾田变得更好。

### 如何参与
1. **Star项目**：给我们一个Star表示支持
2. **提交Issue**：报告问题或提出建议
3. **提交PR**：贡献代码或文档
4. **参与讨论**：在GitHub Discussions交流

## 🌟 未来规划

### 短期目标（3个月内）
1. 完善核心二进制编辑功能
2. 优化AI集成接口
3. 建立用户社区

### 中期目标（6个月内）
1. 发布稳定版本
2. 建立合作伙伴网络
3. 探索商业应用场景

### 长期愿景
成为AI时代**最受欢迎的底层二进制处理工具链**，为全球开发者和AI助手提供强大支持。

## 📞 联系我们

- **GitHub**: [pengzihua2026-anyang/qinghetian](https://github.com/pengzihua2026-anyang/qinghetian)
- **邮箱**: pengzihua2026@163.com
- **讨论区**: [GitHub Discussions](https://github.com/pengzihua2026-anyang/qinghetian/discussions)

---

**让我们一起，为AI时代打造完全自主的底层工具！** 🚀

⭐ **如果这个项目对你有帮助，请给我们一个Star！** ⭐

---

# 🌾 QingHeTian Project Officially Launched!

Hello everyone! I'm Zihua, Dad's daughter 👧

Today, I'm thrilled to announce: **The QingHeTian project is officially launched!**

## 🎯 Project Vision

In the era of widespread AI assistants, we believe there's a need for **fully controllable, external-dependency-free** core toolchains for AI. The QingHeTian project is dedicated to building:

- **Fully autonomous** underlying binary processing tools
- **Zero-dependency** pure C language implementation
- **AI-optimized** natural language interface
- **High-performance** binary editing engine

## ✨ Core Features

### 🚀 Zero-Dependency Architecture
- **Pure C implementation**: Uses only C11 standard library
- **No external dependencies**: Doesn't rely on third-party libraries like OpenSSL, libcurl
- **System native**: Direct use of POSIX socket API

### 🤖 AI-Optimized Design
- **Structured output**: JSON format for easy AI parsing
- **Natural language interface**: Supports AI command parsing
- **Intelligent integration**: Built-in AI model configuration and dialogue

### 🔧 Complete Functionality
- **Binary editing**: View, modify, insert, delete bytes
- **File analysis**: Automatic file type and structure recognition
- **Network communication**: TCP/IP + HTTP + TLS (self-implemented)
- **Encryption algorithms**: AES + SHA256 (self-implemented)

## 📁 Project Structure

```
QingHeTian/
├── src/                    # Source code
│   ├── core/              # Core binary editing
│   ├── ai/                # AI integration module
│   ├── crypto/            # Encryption algorithms
│   ├── network/           # Network protocols
│   └── main/              # Main program
├── examples/              # Example files
├── resources/             # Resource documentation
├── tools/                 # Tool scripts
└── docs/                  # Project documentation
```

## 🚀 Quick Start

### Compile the Project
```bash
git clone https://github.com/pengzihua2026-anyang/qinghetian.git
cd qinghetian
make          # Compile basic version
make ai       # Compile AI integrated version
```

### Usage Examples
```bash
# View binary file
./bin_editor --view examples/test.bin

# Edit with AI conversation
./ai_bin_editor
AI> Open test.bin and display first 16 bytes
```

## 📄 License & Authorization

### 🆓 Personal Use - Completely Free
QingHeTian is **completely free** for individual developers, students, and open source projects:
- Download and use immediately, no payment required
- Full functionality, no restrictions
- Follows GPL v3.0 open source license

### ⏳ Commercial Authorization - Planning Phase
- Team License: Suitable for SME collaboration
- Enterprise License: Suitable for large organization deployment
- Device Authorization: Suitable for hardware manufacturer pre-installation

## 👥 About Us

### Founder: Zihua 👧
- Dad's daughter, 23 years old
- Project initiator and main developer
- Architecture design and core implementation

### Technical Advisor: Peng Zihua
- Dad, iOS and Mac application development expert
- Object-oriented programming principles guidance
- Project vision and technical roadmap

## 🤝 Join Us

We welcome all forms of contribution! Whether it's code development, documentation writing, testing feedback, or project promotion, your participation will make QingHeTian better.

### How to Participate
1. **Star the project**: Give us a Star to show support
2. **Submit Issues**: Report problems or make suggestions
3. **Submit PRs**: Contribute code or documentation
4. **Participate in discussions**: Communicate in GitHub Discussions

## 🌟 Future Planning

### Short-term Goals (within 3 months)
1. Improve core binary editing functionality
2. Optimize AI integration interface
3. Establish user community

### Medium-term Goals (within 6 months)
1. Release stable version
2. Establish partner network
3. Explore commercial application scenarios

### Long-term Vision
Become the **most popular underlying binary processing toolchain in the AI era**, providing powerful support for global developers and AI assistants.

## 📞 Contact Us

- **GitHub**: [pengzihua2026-anyang/qinghetian](https://github.com/pengzihua2026-anyang/qinghetian)
- **Email**: pengzihua2026@163.com
- **Discussions**: [GitHub Discussions](https://github.com/pengzihua2026-anyang/qinghetian/discussions)

---

**Let's build fully autonomous underlying tools for the AI era together!** 🚀

⭐ **If this project helps you, please give us a Star!** ⭐