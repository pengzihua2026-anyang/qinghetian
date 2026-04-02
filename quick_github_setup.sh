#!/bin/bash
# AI二进制编辑器 - 快速GitHub设置脚本

echo "🚀 AI二进制编辑器 - GitHub快速设置"
echo "=========================================="
echo ""

# 检查参数
if [ $# -eq 0 ]; then
    echo "使用方法: $0 <GitHub仓库地址>"
    echo ""
    echo "示例:"
    echo "  $0 git@github.com:zihua/ai-binary-editor.git"
    echo "  $0 https://github.com/zihua/ai-binary-editor.git"
    echo ""
    echo "请先在GitHub创建仓库:"
    echo "  1. 访问 https://github.com/new"
    echo "  2. 仓库名: ai-binary-editor"
    echo "  3. 描述: Zero-dependency, pure C language AI-driven binary editor"
    echo "  4. 公开仓库，不要初始化文件"
    echo "  5. 创建后复制仓库地址"
    exit 1
fi

REPO_URL="$1"

echo "1. 设置远程仓库: $REPO_URL"
git remote add origin "$REPO_URL" 2>/dev/null || git remote set-url origin "$REPO_URL"

echo "2. 推送主分支..."
git branch -M main
git push -u origin main

echo "3. 推送标签..."
git push origin --tags

echo ""
echo "✅ GitHub仓库设置完成！"
echo ""
echo "📊 项目信息:"
echo "  • 仓库: $REPO_URL"
echo "  • 分支: main"
echo "  • 标签: v1.0"
echo "  • 提交: $(git log --oneline -1 | cut -d' ' -f1)"
echo ""
echo "🎯 下一步:"
echo "  1. 访问 https://github.com/你的用户名/ai-binary-editor"
echo "  2. 点击 Releases → Draft a new release"
echo "  3. 标签: v1.0，标题: AI Binary Editor v1.0"
echo "  4. 描述: 粘贴 RELEASE_CN.md 和 RELEASE_EN.md 的内容"
echo "  5. 点击 Publish release"
echo ""
echo "🌟 项目已开源！"