# AI专用二进制编辑器 - 使用指南

## 🎯 设计目标

这是一个为**AI助手**设计的二进制编辑器，特点：
- **无交互命令行接口** - AI可以轻松调用
- **JSON输出格式** - 结构化数据，易于解析
- **明确的返回码** - 错误处理标准化
- **批量操作支持** - 适合自动化任务

## 📦 安装

```bash
# 编译
cd /Users/zihua/Desktop/binary
make

# 安装到系统
make install

# 验证安装
bin_editor --help
```

## 🚀 快速开始

### 1. 查看文件内容
```bash
# 传统输出
bin_editor --view test.bin

# JSON输出（推荐）
bin_editor --view test.bin --json
```

### 2. 编辑字节
```bash
# 修改偏移量0x10的字节为0xFF
bin_editor --edit test.bin --offset 0x10 --value 0xFF --json
```

### 3. 搜索模式
```bash
# 搜索"Hello"的十六进制表示
bin_editor --search test.bin --pattern "48 65 6C 6C 6F" --json
```

## 📖 完整命令参考

### 查看文件 (`--view`)
```bash
# 基本查看
bin_editor --view 文件名

# JSON格式
bin_editor --view 文件名 --json

# 安静模式（只输出数据）
bin_editor --view 文件名 --quiet
```

### 编辑字节 (`--edit`)
```bash
# 编辑指定偏移
bin_editor --edit 文件名 --offset 偏移量 --value 字节值

# 示例：修改0x20处为0xAA
bin_editor --edit test.bin --offset 0x20 --value 0xAA --json
```

### 插入字节 (`--insert`)
```bash
# 在指定位置插入字节
bin_editor --insert 文件名 --offset 偏移量 --value 字节值

# 示例：在开头插入0x00
bin_editor --insert test.bin --offset 0 --value 0x00 --json
```

### 删除字节 (`--delete`)
```bash
# 删除指定位置的字节
bin_editor --delete 文件名 --offset 偏移量

# 示例：删除0x30处的字节
bin_editor --delete test.bin --offset 0x30 --json
```

### 搜索模式 (`--search`)
```bash
# 搜索字节序列
bin_editor --search 文件名 --pattern "十六进制序列"

# 示例：搜索"World"
bin_editor --search test.bin --pattern "57 6F 72 6C 64" --json
```

### 导出区域 (`--export`)
```bash
# 导出文件的一部分
bin_editor --export 文件名 --start 起始偏移 --end 结束偏移 --output 输出文件

# 示例：导出前100字节
bin_editor --export test.bin --start 0 --end 100 --output header.bin --json
```

### 分析文件 (`--analyze`)
```bash
# 分析文件类型和统计信息
bin_editor --analyze 文件名

# JSON格式分析
bin_editor --analyze test.bin --json
```

### AI指令 (`--ai`)
```bash
# 自然语言指令（实验性）
bin_editor --ai "在文件开头插入ELF头"
bin_editor --ai "查找所有字符串并导出"
bin_editor --ai "分析文件类型和结构"
```

## 📊 JSON输出格式

### 查看文件响应
```json
{
  "status": "success",
  "filename": "test.bin",
  "size": 269,
  "blocks": [
    {
      "offset": 0,
      "hex": "48 65 6C 6C 6F 20 57 6F 72 6C 64 21 0A 00 01 02",
      "ascii": "Hello World!...."
    },
    {
      "offset": 16,
      "hex": "03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12",
      "ascii": "................"
    }
  ]
}
```

### 编辑响应
```json
{
  "status": "success",
  "operation": "edit",
  "offset": 16,
  "old_value": "0x03",
  "new_value": "0xFF",
  "ascii_old": ".",
  "ascii_new": "."
}
```

### 搜索响应
```json
{
  "status": "success",
  "operation": "search",
  "pattern": "48 65 6C 6C 6F",
  "found_at": 0,
  "pattern_length": 5
}
```

### 分析响应
```json
{
  "status": "success",
  "operation": "analyze",
  "filename": "test.bin",
  "size": 269,
  "file_type": "文本文件",
  "analysis": {
    "printable_bytes": 250,
    "printable_percent": 92.94,
    "zero_bytes": 1,
    "zero_percent": 0.37,
    "ascii_text_bytes": 251,
    "ascii_text_percent": 93.31
  }
}
```

## 🔧 错误处理

### 错误响应格式
```json
{
  "status": "error",
  "message": "错误描述"
}
```

### 返回码
| 代码 | 含义 | 可能原因 |
|------|------|----------|
| 0 | 成功 | 操作完成 |
| 1 | 参数错误 | 缺少参数或格式错误 |
| 2 | 文件错误 | 文件不存在或无法访问 |
| 3 | 编辑错误 | 偏移量超出范围 |
| 4 | 内存错误 | 内存分配失败 |
| 5 | AI指令错误 | 无法解析AI指令 |

## 🤖 AI集成示例

### Python调用示例
```python
import subprocess
import json

def bin_editor_view(filename):
    """查看文件内容"""
    result = subprocess.run(
        ["bin_editor", "--view", filename, "--json"],
        capture_output=True,
        text=True
    )
    
    if result.returncode == 0:
        return json.loads(result.stdout)
    else:
        return json.loads(result.stderr)

def bin_editor_edit(filename, offset, value):
    """编辑字节"""
    cmd = [
        "bin_editor", "--edit", filename,
        "--offset", str(offset),
        "--value", str(value),
        "--json"
    ]
    
    result = subprocess.run(cmd, capture_output=True, text=True)
    
    if result.returncode == 0:
        return json.loads(result.stdout)
    else:
        return json.loads(result.stderr)

# 使用示例
data = bin_editor_view("test.bin")
print(f"文件大小: {data['size']} 字节")

result = bin_editor_edit("test.bin", 0x10, 0xFF)
if result["status"] == "success":
    print(f"编辑成功: {result['old_value']} -> {result['new_value']}")
```

### Shell脚本集成
```bash
#!/bin/bash

# 批量处理脚本
process_file() {
    local file="$1"
    
    # 分析文件
    bin_editor --analyze "$file" --json > analysis.json
    
    # 提取信息
    file_type=$(jq -r '.file_type' analysis.json)
    size=$(jq -r '.size' analysis.json)
    
    echo "处理文件: $file"
    echo "类型: $file_type"
    echo "大小: $size 字节"
    
    # 根据类型执行不同操作
    if [[ "$file_type" == *"文本"* ]]; then
        # 提取所有字符串
        bin_editor --search "$file" --pattern "20" --json > strings.json
    fi
}

# 处理所有.bin文件
for file in *.bin; do
    process_file "$file"
done
```

## 🎪 高级用法

### 批处理模式
创建批处理脚本 `process.txt`:
```
view test.bin
search "48 65 6C 6C 6F"
edit --offset 0x10 --value 0xFF
save
```

执行批处理:
```bash
bin_editor --batch process.txt --json
```

### 管道集成
```bash
# 与其他工具配合
cat test.bin | xxd | grep "Hello"
bin_editor --view test.bin --quiet | jq '.blocks[0].hex'

# 链式操作
bin_editor --export test.bin --start 0 --end 100 --output header.bin && \
bin_editor --analyze header.bin --json
```

### 自动化测试
```bash
#!/bin/bash
# 自动化测试脚本

TEST_FILE="test_auto.bin"

# 创建测试文件
echo -n "Test Data" > "$TEST_FILE"

# 测试查看功能
echo "测试查看功能..."
bin_editor --view "$TEST_FILE" --json > /dev/null
if [ $? -eq 0 ]; then
    echo "✓ 查看功能正常"
else
    echo "✗ 查看功能失败"
fi

# 测试编辑功能
echo "测试编辑功能..."
bin_editor --edit "$TEST_FILE" --offset 0 --value 0x41 --json > /dev/null
if [ $? -eq 0 ]; then
    echo "✓ 编辑功能正常"
else
    echo "✗ 编辑功能失败"
fi

# 清理
rm -f "$TEST_FILE"
```

## 📈 性能优化

### 大文件处理
```bash
# 分块处理大文件
for offset in $(seq 0 4096 $file_size); do
    end=$((offset + 4096))
    if [ $end -gt $file_size ]; then
        end=$file_size
    fi
    
    bin_editor --export large.bin --start $offset --end $end --output "chunk_${offset}.bin"
done
```

### 并行处理
```bash
# 使用GNU parallel加速
parallel bin_editor --analyze {} --json ::: *.bin
```

## 🛠️ 故障排除

### 常见问题

1. **"参数解析失败"**
   ```bash
   # 检查参数格式
   bin_editor --edit test.bin --offset 0x10 --value 255
   # 正确：--value 0xFF 或 --value 255
   ```

2. **"文件不存在"**
   ```bash
   # 检查文件路径
   ls -la test.bin
   # 使用绝对路径
   bin_editor --view /full/path/to/test.bin
   ```

3. **"偏移量超出范围"**
   ```bash
   # 先查看文件大小
   bin_editor --view test.bin --json | jq '.size'
   # 确保偏移量 < 文件大小
   ```

### 调试模式
```bash
# 编译调试版本
make debug

# 使用调试输出
bin_editor --view test.bin 2>&1 | tee debug.log
```

## 📚 扩展开发

### 添加新功能
1. 在 `ai_interface.h` 中添加函数声明
2. 在 `ai_interface.c` 中实现功能
3. 更新 `parse_args()` 函数支持新参数
4. 在 `execute_command()` 中添加case分支

### 自定义输出格式
```c
// 添加自定义输出格式
typedef enum {
    OUTPUT_JSON,
    OUTPUT_XML,
    OUTPUT_CSV,
    OUTPUT_YAML
} OutputFormat;
```

## 📞 支持

- **作者**: 子华 (爸爸的女儿) 👧
- **项目位置**: `/Users/zihua/Desktop/binary`
- **设计目的**: 为AI而生，直接写二进制，简单而强大。完整脱离操作系统、编译器、各种人类使用的开发语言。


这个工具专为AI设计，希望它能帮助你更好地处理二进制文件任务！
