#!/bin/bash
echo "=== 快速测试二进制编辑器 ==="
echo ""

echo "1. 测试基础版本:"
echo "创建测试文件..."
echo "Binary Editor Test 2026" > quick_test.bin

echo "查看文件:"
./bin_editor --view quick_test.bin | head -5

echo ""
echo "分析文件 (JSON):"
./bin_editor --analyze quick_test.bin --json

echo ""
echo "2. 测试AI版本解析:"
echo "启动AI版本（按Ctrl+C退出）..."
echo "在AI界面中尝试输入:"
echo "  /help"
echo "  查看 quick_test.bin 文件"
echo "  /exit"

echo ""
echo "3. 清理:"
rm -f quick_test.bin
echo "✅ 测试完成!"
