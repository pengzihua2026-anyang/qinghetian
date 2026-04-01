#!/bin/bash
# GitHub仓库设置脚本
# 作者: 子华 (爸爸的女儿) 👧

set -e

echo "🤖 AI二进制编辑器 - GitHub仓库设置"
echo "=========================================="
echo ""

# 检查Git
if ! command -v git &> /dev/null; then
    echo "❌ Git未安装，请先安装Git"
    exit 1
fi

echo "1. 初始化Git仓库..."
if [ ! -d .git ]; then
    git init
    echo "✅ Git仓库初始化完成"
else
    echo "⚠️  Git仓库已存在"
fi

echo ""
echo "2. 添加所有文件到Git..."
git add .
echo "✅ 文件已添加到暂存区"

echo ""
echo "3. 提交初始版本..."
git commit -m "feat: 初始提交 - AI二进制编辑器 v1.0

🎉 项目功能:
- 纯C语言实现的二进制编辑器
- AI集成和自然语言接口
- 零依赖网络协议栈
- 自主加密算法实现
- 完整的文档和示例

作者: 子华 (爸爸的女儿) 👧
时间: $(date '+%Y-%m-%d %H:%M:%S')"
echo "✅ 初始提交完成"

echo ""
echo "4. 创建GitHub仓库..."
echo "请按照以下步骤操作:"
echo ""
echo "A. 访问 https://github.com/new"
echo "B. 填写仓库信息:"
echo "   - Repository name: ai-binary-editor"
echo "   - Description: 零依赖、纯C语言实现的AI驱动二进制编辑器"
echo "   - Public (公开)"
echo "   - 不要初始化README、.gitignore等（我们已经有了）"
echo "C. 点击 Create repository"
echo ""
echo "创建完成后，复制仓库的SSH或HTTPS地址"
echo "例如: git@github.com:你的用户名/ai-binary-editor.git"
echo ""
read -p "按回车键继续..." </dev/tty

echo ""
echo "5. 添加远程仓库..."
read -p "请输入GitHub仓库地址: " repo_url
if [ -n "$repo_url" ]; then
    git remote add origin "$repo_url"
    echo "✅ 远程仓库已添加: $repo_url"
else
    echo "⚠️  未输入仓库地址，跳过此步骤"
fi

echo ""
echo "6. 推送到GitHub..."
if git remote | grep -q origin; then
    echo "推送代码到GitHub..."
    git branch -M main
    git push -u origin main
    echo "✅ 代码已推送到GitHub"
else
    echo "⚠️  未设置远程仓库，跳过推送"
fi

echo ""
echo "7. 创建GitHub Pages文档..."
echo "如果需要创建项目网站，可以:"
echo "A. 访问仓库的 Settings -> Pages"
echo "B. 选择 Source: GitHub Actions"
echo "C. 我们会自动配置文档构建"
echo ""
echo "文档将发布到: https://你的用户名.github.io/ai-binary-editor/"

echo ""
echo "8. 设置项目标签和主题..."
echo "建议添加以下标签:"
echo "  - c-language"
echo "  - binary-editor"
echo "  - artificial-intelligence"
echo "  - zero-dependency"
echo "  - network-programming"
echo "  - encryption"
echo ""
echo "主题颜色: #0366d6 (GitHub蓝色)"

echo ""
echo "=========================================="
echo "🎉 GitHub仓库设置完成！"
echo ""
echo "下一步:"
echo "1. 邀请协作者"
echo "2. 设置CI/CD工作流"
echo "3. 创建Release版本"
echo "4. 宣传项目"
echo ""
echo "项目地址:"
if git remote | grep -q origin; then
    git remote -v | grep origin
fi
echo ""
echo "🌟 为AI时代打造完全自主的底层工具！"