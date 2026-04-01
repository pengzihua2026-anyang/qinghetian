#!/bin/bash
echo "=== 快速AI对话测试（修复后）==="
echo ""

cd "/Users/zihua/Desktop/binary"

echo "1. 确保代理服务器运行..."
if ! ps aux | grep -q "proxy_server.py"; then
    echo "启动代理服务器..."
    python3 proxy_server.py 8080 > /tmp/proxy_test.log 2>&1 &
    sleep 3
    echo "代理服务器启动"
fi

echo ""
echo "2. 运行AI二进制编辑器（简化测试）..."
echo "注意：这个测试需要你手动输入"
echo ""
echo "预期流程:"
echo "1. 程序检测到保存的配置"
echo "2. 加载配置"
echo "3. 输入 /chat 你好"
echo "4. 观察响应"
echo ""
echo "如果看到 'AI: {\"error\": ...}' 说明代理仍有问题"
echo "如果看到真正的AI回复，说明修复成功"
echo ""
echo "现在运行: ./ai_bin_editor"
echo ""
echo "按 Ctrl+C 停止测试"
echo ""

# 显示代理状态
echo "代理服务器状态:"
ps aux | grep "proxy_server" | grep -v grep
echo ""
echo "要查看代理日志: tail -f /tmp/proxy_test.log"
echo "要停止代理: kill \$(ps aux | grep 'proxy_server' | grep -v grep | awk '{print \$2}')"