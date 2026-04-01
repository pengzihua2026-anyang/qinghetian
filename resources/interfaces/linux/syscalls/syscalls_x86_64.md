# Linux x86-64 系统调用参考

## 📊 系统调用概览

### 调用约定
```
系统调用号: RAX
参数 1-6: RDI, RSI, RDX, R10, R8, R9
返回值: RAX
错误码: -errno (负值)
调用指令: syscall
```

### 寄存器保存
- `syscall` 指令不自动保存任何寄存器
- `rcx` 被破坏 (保存返回地址)
- `r11` 被破坏 (保存RFLAGS)
- 其他寄存器由调用者保存

## 📋 常用系统调用表

### 文件操作

| 编号 | 名称 | 描述 | 参数 |
|------|------|------|------|
| 0 | read | 从文件描述符读取 | fd, buf, count |
| 1 | write | 写入文件描述符 | fd, buf, count |
| 2 | open | 打开文件 | pathname, flags, mode |
| 3 | close | 关闭文件描述符 | fd |
| 8 | lseek | 重新定位读/写偏移 | fd, offset, whence |
| 9 | mmap | 内存映射文件 | addr, length, prot, flags, fd, offset |
| 10 | mprotect | 设置内存保护 | addr, len, prot |
| 11 | munmap | 取消内存映射 | addr, length |
| 17 | pread64 | 从指定偏移读取 | fd, buf, count, offset |
| 18 | pwrite64 | 写入指定偏移 | fd, buf, count, offset |
| 19 | readv | 从多个缓冲区读取 | fd, iov, iovcnt |
| 20 | writev | 写入多个缓冲区 | fd, iov, iovcnt |
| 21 | access | 检查文件访问权限 | pathname, mode |
| 22 | pipe | 创建管道 | pipefd |
| 32 | dup | 复制文件描述符 | oldfd |
| 33 | dup2 | 复制文件描述符到指定值 | oldfd, newfd |
| 49 | bind | 绑定套接字地址 | sockfd, addr, addrlen |
| 50 | listen | 监听套接字连接 | sockfd, backlog |
| 257 | openat | 相对于目录打开文件 | dirfd, pathname, flags, mode |

### 进程管理

| 编号 | 名称 | 描述 | 参数 |
|------|------|------|------|
| 39 | getpid | 获取进程ID | - |
| 56 | clone | 创建子进程 | flags, child_stack, ptid, ctid, regs |
| 57 | fork | 创建子进程 (传统) | - |
| 58 | vfork | 创建子进程 (共享地址空间) | - |
| 59 | execve | 执行程序 | filename, argv, envp |
| 60 | exit | 终止进程 | status |
| 61 | wait4 | 等待进程状态变化 | pid, wstatus, options, rusage |
| 101 | ptrace | 进程跟踪 | request, pid, addr, data |
| 158 | arch_prctl | 架构特定控制 | code, addr |
| 231 | exit_group | 终止所有线程 | status |

### 内存管理

| 编号 | 名称 | 描述 | 参数 |
|------|------|------|------|
| 9 | mmap | 内存映射 | addr, length, prot, flags, fd, offset |
| 10 | mprotect | 设置内存保护 | addr, len, prot |
| 11 | munmap | 取消内存映射 | addr, length |
| 12 | brk | 改变数据段大小 | addr |
| 25 | mremap | 重新映射虚拟内存地址 | old_address, old_size, new_size, flags, new_address |
| 28 | mlock | 锁定内存页 | addr, len |
| 29 | munlock | 解锁内存页 | addr, len |
| 149 | madvise | 提供内存使用建议 | addr, length, advice |
| 155 | set_thread_area | 设置线程本地存储 | u_info |

### 网络操作

| 编号 | 名称 | 描述 | 参数 |
|------|------|------|------|
| 41 | socket | 创建套接字 | domain, type, protocol |
| 42 | connect | 连接套接字 | sockfd, addr, addrlen |
| 43 | accept | 接受连接 | sockfd, addr, addrlen |
| 44 | sendto | 发送消息 | sockfd, buf, len, flags, dest_addr, addrlen |
| 45 | recvfrom | 接收消息 | sockfd, buf, len, flags, src_addr, addrlen |
| 46 | sendmsg | 发送消息 (带控制信息) | sockfd, msg, flags |
| 47 | recvmsg | 接收消息 (带控制信息) | sockfd, msg, flags |
| 48 | shutdown | 关闭套接字连接 | sockfd, how |
| 49 | bind | 绑定地址 | sockfd, addr, addrlen |
| 50 | listen | 监听连接 | sockfd, backlog |
| 51 | getsockname | 获取套接字本地地址 | sockfd, addr, addrlen |
| 52 | getpeername | 获取对端地址 | sockfd, addr, addrlen |
| 53 | socketpair | 创建一对连接的套接字 | domain, type, protocol, sv |
| 54 | setsockopt | 设置套接字选项 | sockfd, level, optname, optval, optlen |
| 55 | getsockopt | 获取套接字选项 | sockfd, level, optname, optval, optlen |

### 信号处理

| 编号 | 名称 | 描述 | 参数 |
|------|------|------|------|
| 13 | rt_sigaction | 检查或更改信号动作 | sig, act, oact, sigsetsize |
| 14 | rt_sigprocmask | 检查或更改阻塞信号 | how, set, oldset, sigsetsize |
| 22 | pause | 等待信号 | - |
| 34 | kill | 发送信号到进程 | pid, sig |
| 35 | tgkill | 发送信号到线程 | tgid, tid, sig |
| 37 | alarm | 设置闹钟 | seconds |
| 127 | rt_sigpending | 检查待处理信号 | set, sigsetsize |
| 129 | rt_sigtimedwait | 同步等待信号 | set, info, timeout, sigsetsize |
| 130 | rt_sigqueueinfo | 排队信号和信息 | pid, sig, info |

### 时间操作

| 编号 | 名称 | 描述 | 参数 |
|------|------|------|------|
| 96 | gettimeofday | 获取时间 | tv, tz |
| 201 | time | 获取秒数时间 | tloc |
| 228 | clock_gettime | 获取时钟时间 | clk_id, tp |
| 229 | clock_settime | 设置时钟时间 | clk_id, tp |
| 230 | clock_getres | 获取时钟分辨率 | clk_id, res |
| 232 | nanosleep | 高精度睡眠 | req, rem |
| 247 | getcpu | 获取CPU和NUMA节点 | cpu, node, tcache |

### 系统信息

| 编号 | 名称 | 描述 | 参数 |
|------|------|------|------|
| 63 | uname | 获取系统信息 | buf |
| 64 | sethostname | 设置主机名 | name, len |
| 65 | setdomainname | 设置域名 | name, len |
| 83 | symlink | 创建符号链接 | target, linkpath |
| 84 | readlink | 读取符号链接内容 | pathname, buf, bufsiz |
| 85 | uselib | 选择共享库 | library |
| 102 | getuid | 获取用户ID | - |
| 104 | getgid | 获取组ID | - |
| 107 | geteuid | 获取有效用户ID | - |
| 108 | getegid | 获取有效组ID | - |
| 116 | sysinfo | 返回系统统计信息 | info |
| 157 | prctl | 进程控制 | option, arg2, arg3, arg4, arg5 |
| 160 | unshare | 取消共享进程上下文 | flags |

## 💻 汇编示例

### 基本系统调用模板
```asm
section .data
    msg db 'Hello, World!', 0xA
    len equ $ - msg

section .text
    global _start

_start:
    ; write(1, msg, len)
    mov rax, 1          ; sys_write
    mov rdi, 1          ; fd = stdout
    mov rsi, msg        ; buffer
    mov rdx, len        ; length
    syscall

    ; exit(0)
    mov rax, 60         ; sys_exit
    xor rdi, rdi        ; status = 0
    syscall
```

### 文件操作示例
```asm
section .data
    filename db 'test.txt', 0
    content db 'Hello File!', 0xA
    content_len equ $ - content

section .bss
    fd resq 1

section .text
    global _start

_start:
    ; open(filename, O_WRONLY|O_CREAT, 0644)
    mov rax, 2          ; sys_open
    mov rdi, filename
    mov rsi, 0x41       ; O_WRONLY|O_CREAT
    mov rdx, 0644o      ; mode
    syscall
    mov [fd], rax       ; 保存文件描述符

    ; write(fd, content, content_len)
    mov rax, 1          ; sys_write
    mov rdi, [fd]
    mov rsi, content
    mov rdx, content_len
    syscall

    ; close(fd)
    mov rax, 3          ; sys_close
    mov rdi, [fd]
    syscall

    ; exit(0)
    mov rax, 60
    xor rdi, rdi
    syscall
```

### 网络编程示例
```asm
section .data
    ; 套接字地址结构
    server_addr:
        dw 2            ; AF_INET
        dw 0x901F       ; port 8080 (网络字节序)
        dd 0            ; INADDR_ANY
        times 8 db 0    ; 填充

section .bss
    sockfd resq 1
    clientfd resq 1

section .text
    global _start

_start:
    ; socket(AF_INET, SOCK_STREAM, 0)
    mov rax, 41         ; sys_socket
    mov rdi, 2          ; AF_INET
    mov rsi, 1          ; SOCK_STREAM
    mov rdx, 0          ; protocol
    syscall
    mov [sockfd], rax

    ; bind(sockfd, &server_addr, 16)
    mov rax, 49         ; sys_bind
    mov rdi, [sockfd]
    mov rsi, server_addr
    mov rdx, 16         ; sizeof(struct sockaddr_in)
    syscall

    ; listen(sockfd, 5)
    mov rax, 50         ; sys_listen
    mov rdi, [sockfd]
    mov rsi, 5          ; backlog
    syscall

    ; accept(sockfd, NULL, NULL)
    mov rax, 43         ; sys_accept
    mov rdi, [sockfd]
    xor rsi, rsi        ; addr = NULL
    xor rdx, rdx        ; addrlen = NULL
    syscall
    mov [clientfd], rax

    ; ... 处理连接 ...

    ; close(clientfd)
    mov rax, 3
    mov rdi, [clientfd]
    syscall

    ; close(sockfd)
    mov rax, 3
    mov rdi, [sockfd]
    syscall

    ; exit(0)
    mov rax, 60
    xor rdi, rdi
    syscall
```

## 🔧 错误处理

### 错误码检查
```asm
    syscall
    test rax, rax       ; 检查返回值
    js .error           ; 如果为负，跳转到错误处理
    ; 正常处理...

.error:
    neg rax             ; 获取正错误码
    ; 根据错误码处理...
```

### 常见错误码
| 错误码 | 名称 | 描述 |
|--------|------|------|
| 1 | EPERM | 操作不允许 |
| 2 | ENOENT | 文件或目录不存在 |
| 5 | EIO | 输入/输出错误 |
| 9 | EBADF | 错误的文件描述符 |
| 11 | EAGAIN | 资源暂时不可用 |
| 13 | EACCES | 权限被拒绝 |
| 14 | EFAULT | 错误的地址 |
| 22 | EINVAL | 无效参数 |
| 24 | EMFILE | 打开文件过多 |
| 32 | EPIPE | 管道破裂 |

## 📝 调用约定细节

### 参数传递
- 最多6个参数通过寄存器传递
- 额外参数通过栈传递 (右到左)
- 浮点参数通过XMM寄存器传递

### 返回值
- 成功: 返回非负值
- 失败: 返回负的错误码 (-errno)
- 某些系统调用返回指针或特殊值

### 寄存器保存
```asm
; 调用前保存易失寄存器
push rcx
push r11
; 系统调用
mov rax, 1
mov rdi, 1
mov rsi, msg
mov rdx, len
syscall
; 恢复寄存器
pop r11
pop rcx
```

## 🚀 性能优化

### 减少系统调用
- 批量操作 (readv/writev)
- 内存映射文件 (mmap)
- 异步I/O

### 避免上下文切换
- 使用用户空间库
- 减少不必要的系统调用
- 使用vDSO (虚拟动态共享对象)

### 缓存友好
- 对齐数据
- 预读数据
- 使用大页面

## 📚 进一步学习

### 官方文档
- Linux内核源码: `arch/x86/entry/syscalls/syscall_64.tbl`
- man pages: `man 2 syscalls`
- Linux编程接口 (书籍)

### 调试工具
```bash
# 跟踪系统调用
strace ./program

# 查看系统调用表
cat /usr/include/asm/unistd_64.h

# 反汇编系统调用
objdump -d /lib/x86_64-linux-gnu/libc.so.6 | grep -A5 "syscall"
```

### 练习项目
1. 编写简单的shell
2. 实现文件复制工具
3. 创建简单的Web服务器
4. 编写进程监控工具

---

*掌握Linux系统调用是编写高效系统程序的基础，理解调用约定和错误处理至关重要。*