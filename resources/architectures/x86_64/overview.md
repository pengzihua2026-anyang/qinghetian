# x86-64 架构概述

## 🏗️ 基本信息

### 发展历史
- **1978**: Intel 8086 (16位)
- **1985**: Intel 80386 (32位)
- **2003**: AMD Opteron (AMD64/64位扩展)
- **2004**: Intel EM64T (兼容AMD64)
- **至今**: 持续演进，添加新指令集扩展

### 别名
- AMD64 (AMD命名)
- Intel 64 (Intel命名)
- x64 (Microsoft命名)
- x86_64 (Unix/Linux命名)

## 🧬 架构特性

### 寄存器组
```
64位通用寄存器 (16个):
  RAX, RBX, RCX, RDX     ; 累加器、基址、计数、数据
  RSI, RDI, RBP, RSP     ; 源索引、目的索引、基指针、栈指针
  R8, R9, R10, R11       ; 额外通用寄存器
  R12, R13, R14, R15     ; 更多通用寄存器

特殊寄存器:
  RIP     ; 指令指针 (Program Counter)
  RFLAGS  ; 标志寄存器
  CR0-CR4 ; 控制寄存器
  DR0-DR7 ; 调试寄存器

段寄存器 (16位):
  CS, DS, ES, FS, GS, SS ; 代码、数据、额外、栈段

浮点/SIMD寄存器:
  XMM0-XMM15  ; 128位 SSE寄存器
  YMM0-YMM15  ; 256位 AVX寄存器
  ZMM0-ZMM31  ; 512位 AVX-512寄存器
```

### 操作模式
1. **实模式** (Real Mode)
   - 16位，1MB地址空间
   - 用于引导和遗留系统

2. **保护模式** (Protected Mode)
   - 32位，4GB地址空间
   - 内存保护，虚拟内存

3. **长模式** (Long Mode)
   - 64位，256TB地址空间
   - 当前主要操作模式
   - 子模式：64位模式、兼容模式

## 💾 内存模型

### 地址空间
- **理论最大**: 2^64 = 16EB (Exabytes)
- **实际实现**: 通常48位 = 256TB
- **规范地址**: 高16位必须为0 (规范形式)

### 内存分页
```
4KB页 (标准):
  虚拟地址: [63:48] = 0
            [47:39] = PML4索引
            [38:30] = 页目录指针索引
            [29:21] = 页目录索引
            [20:12] = 页表索引
            [11:0]  = 页内偏移

2MB大页:
  跳过页表索引，直接使用页目录

1GB巨页:
  跳过页目录和页表索引
```

### 内存保护
- 用户/内核模式 (Ring 0-3)
- 读/写/执行权限
- 不可执行位 (NX/XD)
- 地址空间布局随机化 (ASLR)

## 🔧 指令集扩展

### 基础指令集
- **x86**: 基础32位指令
- **x87**: 浮点协处理器指令
- **MMX**: 多媒体扩展 (64位整数SIMD)

### SIMD扩展
- **SSE** (Streaming SIMD Extensions): 128位浮点/整数
  - SSE, SSE2, SSE3, SSSE3, SSE4.1, SSE4.2
- **AVX** (Advanced Vector Extensions): 256位浮点
  - AVX, AVX2, AVX-512 (512位)
- **FMA** (Fused Multiply-Add): 融合乘加

### 其他扩展
- **AES-NI**: 加密加速
- **SHA-NI**: 哈希加速
- **BMI1/BMI2**: 位操作指令
- **ADX**: 大整数运算
- **RDSEED/RDRAND**: 随机数生成

## 🚀 系统调用机制

### Linux x86-64 系统调用
```asm
; 系统调用号: RAX
; 参数: RDI, RSI, RDX, R10, R8, R9
; 返回值: RAX
; 指令: syscall

; 示例: write(1, "Hello", 5)
mov rax, 1          ; sys_write = 1
mov rdi, 1          ; fd = stdout
mov rsi, message    ; buffer
mov rdx, 5          ; count
syscall
```

### 常用系统调用号
| 名称 | 编号 | 描述 | 参数 |
|------|------|------|------|
| read | 0 | 读文件 | fd, buf, count |
| write | 1 | 写文件 | fd, buf, count |
| open | 2 | 打开文件 | pathname, flags, mode |
| close | 3 | 关闭文件 | fd |
| exit | 60 | 退出进程 | status |
| mmap | 9 | 内存映射 | addr, length, prot, flags, fd, offset |
| munmap | 11 | 取消映射 | addr, length |
| brk | 12 | 调整堆大小 | addr |

### Windows x64 系统调用
- 通过 `syscall` 指令，但调用约定不同
- 系统调用号不固定，通过NTDLL跳转表
- 参数: RCX, RDX, R8, R9, 栈

## 📝 调用约定

### System V AMD64 ABI (Linux/macOS)
```
参数传递:
  整数/指针: RDI, RSI, RDX, RCX, R8, R9
  浮点: XMM0-XMM7
  额外参数: 栈 (右到左)
返回值:
  整数: RAX, RDX
  浮点: XMM0, XMM1
  大结构: 内存 (RDX指向)

被调用者保存:
  RBX, RBP, R12-R15

调用者保存:
  其他所有寄存器

栈对齐:
  16字节对齐 (call指令压入8字节返回地址)
```

### Microsoft x64 Calling Convention (Windows)
```
参数传递:
  整数/指针: RCX, RDX, R8, R9
  浮点: XMM0-XMM3
  额外参数: 栈 (右到左)
  为前4个参数保留栈空间 (shadow space)

返回值:
  整数: RAX
  浮点: XMM0
  大结构: 内存 (RCX指向)

被调用者保存:
  RBX, RBP, RDI, RSI, R12-R15, XMM6-XMM15
```

## 🎯 优化指南

### 通用优化
1. **寄存器使用**: 优先使用易失寄存器
2. **循环展开**: 减少分支预测失败
3. **内存对齐**: 确保数据对齐到自然边界
4. **缓存友好**: 局部性原理，预取

### 指令选择
```asm
; 好: 使用LEA进行算术
lea rax, [rbx + rcx*4 + 10]  ; 单周期

; 不好: 多个指令
mov rax, rcx
shl rax, 2
add rax, rbx
add rax, 10                  ; 多周期
```

### SIMD优化
```asm
; 使用AVX2进行向量加法
vmovdqu ymm0, [src1]
vmovdqu ymm1, [src2]
vpaddd ymm2, ymm0, ymm1     ; 一次处理8个32位整数
vmovdqu [dest], ymm2
```

## 🐛 常见陷阱

### 1. 栈对齐问题
```asm
; 错误: 栈未16字节对齐
push rbx
call function    ; 栈未对齐，可能崩溃

; 正确: 对齐栈
sub rsp, 8       ; 对齐到16字节
push rbx
call function
add rsp, 8
```

### 2. 红色区域 (Red Zone)
- System V ABI: rsp以下128字节为红色区域
- 中断/信号处理程序不会修改
- 可用于临时存储，无需调整rsp
- Windows无此约定

### 3. 指令长度前缀
- REX前缀 (40h-4Fh) 用于64位操作
- 操作数大小前缀 (66h) 用于16位操作
- 地址大小前缀 (67h) 用于32位地址

## 🔧 开发工具

### 汇编器语法
```asm
; NASM语法
section .data
    msg db 'Hello', 0

section .text
    global _start
_start:
    mov rax, 1
    mov rdi, 1
    mov rsi, msg
    mov rdx, 5
    syscall

    mov rax, 60
    xor rdi, rdi
    syscall

; GNU as语法
.section .data
msg: .ascii "Hello"

.section .text
.globl _start
_start:
    mov $1, %rax
    mov $1, %rdi
    mov $msg, %rsi
    mov $5, %rdx
    syscall

    mov $60, %rax
    xor %rdi, %rdi
    syscall
```

### 编译和链接
```bash
# NASM编译
nasm -f elf64 program.asm -o program.o
ld program.o -o program

# GNU as编译
as program.s -o program.o
ld program.o -o program

# 查看反汇编
objdump -d program
```

## 📚 进一步学习

### 官方文档
- Intel® 64 and IA-32 Architectures Software Developer Manuals
- AMD64 Architecture Programmer's Manual

### 实用资源
- [OSDev Wiki x86-64](https://wiki.osdev.org/X86-64)
- [Felix Cloutier x86指令参考](https://www.felixcloutier.com/x86/)
- [x86-64 Assembly Guide](http://cs.lmu.edu/~ray/notes/x86assembly/)

### 练习项目
1. 编写"Hello World"引导程序
2. 实现简单的系统调用包装
3. 创建SIMD优化的矩阵乘法
4. 编写自定义中断处理程序

---

*x86-64是当前最广泛使用的64位架构，理解其细节对于系统编程和性能优化至关重要。*