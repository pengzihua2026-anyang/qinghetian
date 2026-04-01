# 硬件与系统接口文档

## 🎯 接口分类

### 1. **系统调用接口**
- 操作系统内核提供的服务
- 进程管理、文件系统、网络等
- 不同操作系统有不同实现

### 2. **硬件抽象接口**
- BIOS/UEFI固件接口
- ACPI电源管理
- PCI/PCIe设备配置

### 3. **设备驱动接口**
- 字符设备、块设备、网络设备
- 中断处理、DMA、IO端口
- 设备树、ACPI表

### 4. **网络协议栈**
- TCP/IP协议族
- 套接字接口
- 网络设备驱动

### 5. **文件系统接口**
- 虚拟文件系统 (VFS)
- 块设备接口
- 文件操作API

## 🖥️ 操作系统接口

### Linux 系统调用
```
位置: interfaces/linux/syscalls/
├── syscalls_64.md      # x86-64系统调用表
├── syscalls_32.md      # x86-32系统调用表
├── syscalls_arm64.md   # ARM64系统调用表
└── examples/          # 系统调用示例
```

### Windows API
```
位置: interfaces/windows/
├── win32_api.md       # Win32 API参考
├── nt_api.md          # Native API (NTDLL)
├── kernel_driver.md   # 内核驱动接口
└── examples/         # Windows编程示例
```

### macOS/iOS (Darwin)
```
位置: interfaces/darwin/
├── mach_traps.md      # Mach陷阱 (系统调用)
├── bsd_syscalls.md    # BSD系统调用
├── cocoa.md          # Cocoa框架
└── examples/         # macOS编程示例
```

## 🔌 硬件接口

### BIOS 接口
```asm
; BIOS中断调用 (实模式)
mov ah, 0x0E          ; 显示字符功能
mov al, 'A'           ; 字符
int 0x10              ; 视频服务中断
```

### UEFI 接口
```c
// UEFI应用程序示例
EFI_STATUS EFIAPI UefiMain(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Hello UEFI!\n");
    return EFI_SUCCESS;
}
```

### ACPI 接口
- 高级配置与电源接口
- 系统描述表 (SDT)
- 电源管理、热管理、设备配置

### PCI/PCIe 配置
```c
// PCI设备配置空间访问
uint32_t pci_read_config(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t address = (1 << 31) | (bus << 16) | (device << 11) | (function << 8) | (offset & 0xFC);
    outl(0xCF8, address);
    return inl(0xCFC);
}
```

## 📡 网络接口

### 套接字编程
```c
// TCP服务器示例
int server_fd = socket(AF_INET, SOCK_STREAM, 0);
struct sockaddr_in addr = {
    .sin_family = AF_INET,
    .sin_port = htons(8080),
    .sin_addr.s_addr = INADDR_ANY
};
bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
listen(server_fd, 5);
```

### 网络设备驱动
```c
// Linux网络设备操作
struct net_device *dev = alloc_netdev(0, "eth0", NET_NAME_UNKNOWN, ether_setup);
dev->netdev_ops = &my_netdev_ops;
register_netdev(dev);
```

## 💾 存储接口

### 块设备接口
```c
// Linux块设备操作
struct block_device_operations my_blk_ops = {
    .owner = THIS_MODULE,
    .open = my_blk_open,
    .release = my_blk_release,
    .ioctl = my_blk_ioctl,
};
```

### 文件系统接口
```c
// 虚拟文件系统操作
struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .read = my_fs_read,
    .write = my_fs_write,
    .open = my_fs_open,
    .release = my_fs_release,
};
```

## 🎮 图形接口

### VGA/VBE 文本模式
```asm
; VGA文本模式内存映射
; 地址: 0xB8000
; 格式: [字符][属性]
mov ax, 0xB800
mov es, ax
mov byte [es:0], 'H'    ; 字符
mov byte [es:1], 0x0F   ; 属性 (白字黑底)
```

### 帧缓冲 (Framebuffer)
```c
// Linux帧缓冲访问
int fb_fd = open("/dev/fb0", O_RDWR);
struct fb_var_screeninfo vinfo;
ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);
void *fb_mem = mmap(NULL, vinfo.yres_virtual * vinfo.xres_virtual * 2,
                    PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
```

## 🔊 音频接口

### PC Speaker
```asm
; PC喇叭发声
mov al, 0xB6
out 0x43, al           ; 设置定时器模式
mov ax, 1193           ; 频率 = 1.19318MHz / 1193 ≈ 1000Hz
out 0x42, al           ; 发送低字节
mov al, ah
out 0x42, al           ; 发送高字节
in al, 0x61
or al, 0x03
out 0x61, al           ; 打开喇叭
```

### 声卡接口
- AC'97 或 HD Audio
- DMA传输音频数据
- 混音器控制

## 🔧 开发指南

### 接口选择原则
1. **标准化优先**: 使用标准接口而非专有接口
2. **抽象层次**: 选择适当抽象层次 (硬件/驱动/系统/应用)
3. **可移植性**: 考虑跨平台兼容性
4. **性能需求**: 权衡便利性和性能

### 安全考虑
1. **权限检查**: 验证调用者权限
2. **输入验证**: 验证所有参数
3. **边界检查**: 防止缓冲区溢出
4. **错误处理**: 妥善处理所有错误情况

### 调试技巧
1. **日志记录**: 详细记录接口调用
2. **参数检查**: 验证接口参数有效性
3. **性能分析**: 测量接口调用开销
4. **兼容性测试**: 测试不同硬件/系统版本

## 📚 接口文档格式

### 标准文档结构
```markdown
# 接口名称

## 功能描述
- 接口用途
- 适用场景
- 限制条件

## 调用方式
- 函数原型/指令格式
- 参数说明
- 返回值

## 示例代码
```语言
// 示例代码
```

## 错误处理
- 错误代码
- 异常情况
- 恢复方法

## 平台差异
- 不同操作系统实现
- 不同硬件差异
- 版本兼容性

## 性能考虑
- 调用开销
- 内存使用
- 并发处理

## 安全注意事项
- 权限要求
- 输入验证
- 潜在风险
```

### 机器可读格式 (JSON)
```json
{
  "interface": {
    "name": "sys_write",
    "description": "写数据到文件描述符",
    "platform": "linux",
    "architecture": "x86_64",
    "syscall_number": 1,
    "parameters": [
      {"name": "fd", "type": "int", "description": "文件描述符"},
      {"name": "buf", "type": "const void*", "description": "数据缓冲区"},
      {"name": "count", "type": "size_t", "description": "字节数"}
    ],
    "return": {"type": "ssize_t", "description": "写入的字节数，错误时返回-1"},
    "errors": [
      {"code": "EBADF", "description": "文件描述符无效"},
      {"code": "EFAULT", "description": "缓冲区地址无效"}
    ],
    "examples": [
      {"language": "asm", "code": "mov rax, 1\nmov rdi, 1\nmov rsi, msg\nmov rdx, len\nsyscall"}
    ]
  }
}
```

## 🚀 快速开始

### 查找接口
```bash
# 搜索网络相关接口
find interfaces/ -name "*.md" -type f | xargs grep -l "socket\|network"

# 查看Linux系统调用
cat interfaces/linux/syscalls/syscalls_64.md | head -20

# 查找示例代码
find interfaces/ -path "*/examples/*" -name "*.asm" -o -name "*.c"
```

### 使用接口
1. 确定需求对应的接口类别
2. 查阅相关接口文档
3. 参考示例代码
4. 测试和调试

### 贡献接口文档
1. 选择正确的分类目录
2. 按照标准格式编写文档
3. 提供完整示例代码
4. 测试文档准确性

## 📞 支持与更新

### 接口更新
- 关注操作系统版本更新
- 跟踪硬件规范变化
- 及时更新文档

### 问题反馈
- 报告接口文档错误
- 请求新接口文档
- 提供改进建议

### 社区贡献
- 添加缺失的接口文档
- 完善示例代码
- 翻译或本地化

---

*硬件和系统接口是二进制开发的基础，理解这些接口能够编写高效、可靠的底层代码。*