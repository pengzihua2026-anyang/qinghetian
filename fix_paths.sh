#!/bin/bash
# 修复Xcode项目文件路径

PROJECT_FILE="binary.xcodeproj/project.pbxproj"
BACKUP_FILE="binary.xcodeproj/project.pbxproj.backup"

echo "🔧 修复Xcode项目文件路径..."
echo ""

# 备份原文件
cp "$PROJECT_FILE" "$BACKUP_FILE"
echo "✅ 备份完成: $BACKUP_FILE"

# 更新文件路径
echo "正在更新文件路径..."

# 使用sed更新路径
sed -i '' 's|path = main.c;|path = src/main/main.c;|g' "$PROJECT_FILE"
sed -i '' 's|path = hex_editor.c;|path = src/core/hex_editor.c;|g' "$PROJECT_FILE"
sed -i '' 's|path = hex_editor.h;|path = src/core/hex_editor.h;|g' "$PROJECT_FILE"
sed -i '' 's|path = ai_interface.c;|path = src/ai/ai_interface.c;|g' "$PROJECT_FILE"
sed -i '' 's|path = ai_interface.h;|path = src/ai/ai_interface.h;|g' "$PROJECT_FILE"
sed -i '' 's|path = ai_integration.c;|path = src/ai/ai_integration.c;|g' "$PROJECT_FILE"
sed -i '' 's|path = ai_integration.h;|path = src/ai/ai_integration.h;|g' "$PROJECT_FILE"
sed -i '' 's|path = ai_main.c;|path = src/main/ai_main.c;|g' "$PROJECT_FILE"
sed -i '' 's|path = ai_main_new.c;|path = src/main/ai_main_new.c;|g' "$PROJECT_FILE"
sed -i '' 's|path = ai_tcp.c;|path = src/ai/ai_tcp.c;|g' "$PROJECT_FILE"
sed -i '' 's|path = ai_proxy.c;|path = src/ai/ai_proxy.c;|g' "$PROJECT_FILE"
sed -i '' 's|path = aes_impl.c;|path = src/crypto/aes_simple.c;|g' "$PROJECT_FILE"
sed -i '' 's|path = aes_simple.c;|path = src/crypto/aes_simple.c;|g' "$PROJECT_FILE"
sed -i '' 's|path = sha256_impl.c;|path = src/crypto/sha256_impl.c;|g' "$PROJECT_FILE"
sed -i '' 's|path = simple_tls.c;|path = src/network/simple_tls.c;|g' "$PROJECT_FILE"
sed -i '' 's|path = tls_prf.c;|path = src/crypto/tls_prf.c;|g' "$PROJECT_FILE"
sed -i '' 's|path = mini_aes.c;|path = src/crypto/aes_simple.c;|g' "$PROJECT_FILE"
sed -i '' 's|path = mini_tls.h;|path = src/network/mini_tls.h;|g' "$PROJECT_FILE"

echo "✅ 文件路径更新完成"
echo ""
echo "🎯 修复完成！"
echo "请重新打开Xcode项目查看效果。"
