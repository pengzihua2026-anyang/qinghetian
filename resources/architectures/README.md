# CPU架构文档

## 🏗️ 支持的架构

### 1. x86-64 (AMD64/Intel 64)
- **厂商**: Intel, AMD
- **位宽**: 64位
- **字节序**: 小端 (Little Endian)
- **应用**: 桌面、服务器、笔记本

### 2. ARM64 (AArch64)
- **厂商**: ARM, Apple, Qualcomm等
- **位宽**: 64位
- **字节序**: 可配置 (通常小端)
- **应用**: 移动设备、嵌入式、服务器

### 3. RISC-V
- **组织**: RISC-V基金会
- **位宽**: 32/64/128位
- **字节序**: 小端
- **应用**: 嵌入式、IoT、学术研究

### 4. MIPS
- **厂商**: MIPS Technologies
- **位宽**: 32/64位
- **字节序**: 可配置
- **应用**: 嵌入式、网络设备

### 5. PowerPC
- **厂商**: IBM, Freescale
- **位宽**: 32/64位
- **字节序**: 大端 (Big Endian)
- **应用**: 服务器、嵌入式、游戏机

## 📚 文档结构

每个架构目录包含：
```
architectures/<arch-name>/
├── overview.md          # 架构概述
├── registers.md         # 寄存器定义
├── instructions.md      # 指令集参考
├── memory.md           # 内存模型
├── calling_convention.md # 调用约定
├── system_calls.md     # 系统调用
└── examples/           # 示例代码
```

## 🔧 快速参考

### 寄存器对比
| 架构 | 通用寄存器 | 特殊寄存器 | 浮点寄存器 |
|------|------------|------------|------------|
| x86-64 | RAX, RBX, RCX, RDX, RSI, RDI, RBP, RSP, R8-R15 | RIP, RFLAGS | XMM0-XMM15 |
| ARM64 | X0-X30 | SP, PC, PSTATE | V0-V31 |
| RISC-V | X0-X31 | PC, CSR寄存器 | F0-F31 |

### 指令格式
```asm
; x86-64
mov rax, 0x12345678      ; 移动立即数到寄存器

; ARM64
mov x0, #0x12345678      ; 移动立即数到寄存器

; RISC-V
li a0, 0x12345678        # 加载立即数到寄存器
```

### 系统调用方式
```asm
; Linux x86-64
mov rax, 1               ; sys_write
mov rdi, 1               ; fd = stdout
mov rsi, message         ; buffer
mov rdx, length          ; count
syscall

; Linux ARM64
mov x8, 64               ; sys_write
mov x0, 1                ; fd = stdout
mov x1, message          ; buffer
mov x2, length           ; count
svc 0

; Linux RISC-V
li a7, 64                # sys_write
li a0, 1                 # fd = stdout
la a1, message           # buffer
li a2, length            # count
ecall
```

## 🎯 选择指南

### 何时选择x86-64
- 开发桌面应用程序
- 需要最大软件兼容性
- 性能优化重要
- 使用现有x86库

### 何时选择ARM64
- 移动设备或嵌入式系统
- 低功耗要求
- Apple Silicon (M1/M2/M3)
- 服务器工作负载

### 何时选择RISC-V
- 学术研究或教学
- 自定义硬件设计
- 开源项目
- 特定领域加速

### 何时选择其他架构
- **MIPS**: 网络设备、嵌入式系统
- **PowerPC**: 高性能计算、遗留系统
- **其他**: 特定应用场景

## 📖 学习路径

### 初学者
1. 从x86-64开始（文档最丰富）
2. 学习基本指令和寄存器
3. 编写简单汇编程序
4. 理解系统调用机制

### 中级开发者
1. 对比不同架构设计哲学
2. 学习优化技巧
3. 掌握调试工具
4. 理解ABI和调用约定

### 高级专家
1. 深入微架构优化
2. 学习SIMD和向量指令
3. 掌握多核同步
4. 理解虚拟化和安全扩展

## 🔍 架构特性对比

### 性能特性
| 特性 | x86-64 | ARM64 | RISC-V |
|------|--------|-------|--------|
| 乱序执行 | ✅ 强 | ✅ 中等 | ⚠️ 可选 |
| 超标量 | ✅ 是 | ✅ 是 | ⚠️ 可选 |
| SIMD支持 | ✅ AVX/AVX-512 | ✅ NEON/SVE | ⚠️ 向量扩展 |
| 硬件虚拟化 | ✅ VT-x | ✅ 虚拟化扩展 | ⚠️ 超visor扩展 |

### 开发特性
| 特性 | x86-64 | ARM64 | RISC-V |
|------|--------|-------|--------|
| 工具链成熟度 | ✅ 非常高 | ✅ 高 | ⚠️ 中等 |
| 调试支持 | ✅ 完善 | ✅ 良好 | ⚠️ 发展中 |
| 文档丰富度 | ✅ 极丰富 | ✅ 丰富 | ⚠️ 增长中 |
| 社区支持 | ✅ 巨大 | ✅ 大 | ✅ 快速增长 |

### 商业考量
| 考量 | x86-64 | ARM64 | RISC-V |
|------|--------|-------|--------|
| 授权费用 | ⚠️ 高 | ⚠️ 中等 | ✅ 免费 |
| 设计灵活性 | ⚠️ 低 | ⚠️ 中等 | ✅ 高 |
| 生态系统 | ✅ 完整 | ✅ 完整 | ⚠️ 建设中 |
| 长期支持 | ✅ 长期 | ✅ 长期 | ⚠️ 新兴 |

## 🛠️ 工具支持

### 汇编器
- **x86-64**: NASM, YASM, GNU as
- **ARM64**: GNU as, ARM Compiler
- **RISC-V**: GNU as, LLVM

### 调试器
- **通用**: GDB, LLDB
- **架构特定**: Intel VTune, ARM DS-5
- **模拟器**: QEMU, Bochs, Spike (RISC-V)

### 分析工具
- **反汇编**: objdump, radare2, IDA Pro
- **性能分析**: perf, Instruments
- **二进制分析**: binutils, readelf

## 📈 发展趋势

### 当前状态 (2026)
- **x86-64**: 桌面/服务器主导，持续演进
- **ARM64**: 移动主导，向服务器/桌面扩张
- **RISC-V**: 快速增长，嵌入式到高性能计算
- **其他**: 特定领域应用

### 未来展望
- **异构计算**: CPU+GPU+AI加速器
- **领域特定架构**: 针对AI、密码学等优化
- **开源硬件**: RISC-V推动开放标准
- **能效优先**: 低功耗设计越来越重要

## 📞 进一步学习

### 官方文档
- **Intel**: Intel 64 and IA-32 Architectures Software Developer Manuals
- **ARM**: ARM Architecture Reference Manual
- **RISC-V**: RISC-V Specification Documents

### 在线资源
- OSDev Wiki: 操作系统开发百科
- CPU.land: 架构比较和教程
- Wikichip: 微架构数据库

### 书籍推荐
- "Computer Organization and Design" (Patterson & Hennessy)
- "The Art of Assembly Language" (Randall Hyde)
- "ARM Assembly Language" (William Hohl)

---

*选择适合项目需求的架构，理解其特性和限制，是成功二进制开发的关键。*