#!/usr/bin/env python3
"""
修复Xcode项目文件路径
将相对路径更新为新的src/目录结构
"""

import re
import os

def update_xcode_project():
    """更新Xcode项目文件"""
    project_file = "binary.xcodeproj/project.pbxproj"
    backup_file = "binary.xcodeproj/project.pbxproj.backup"
    
    print("🔧 修复Xcode项目文件路径")
    print("=" * 50)
    
    # 读取原文件
    with open(project_file, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # 文件路径映射
    path_mapping = {
        # 旧路径 -> 新路径
        'path = main.c;': 'path = src/main/main.c;',
        'path = hex_editor.c;': 'path = src/core/hex_editor.c;',
        'path = hex_editor.h;': 'path = src/core/hex_editor.h;',
        'path = ai_interface.c;': 'path = src/ai/ai_interface.c;',
        'path = ai_interface.h;': 'path = src/ai/ai_interface.h;',
        'path = ai_integration.c;': 'path = src/ai/ai_integration.c;',
        'path = ai_integration.h;': 'path = src/ai/ai_integration.h;',
        'path = ai_main.c;': 'path = src/main/ai_main.c;',
        'path = ai_main_new.c;': 'path = src/main/ai_main_new.c;',
        'path = ai_tcp.c;': 'path = src/ai/ai_tcp.c;',
        'path = ai_proxy.c;': 'path = src/ai/ai_proxy.c;',
        'path = aes_impl.c;': 'path = src/crypto/aes_simple.c;',
        'path = aes_simple.c;': 'path = src/crypto/aes_simple.c;',
        'path = sha256_impl.c;': 'path = src/crypto/sha256_impl.c;',
        'path = simple_tls.c;': 'path = src/network/simple_tls.c;',
        'path = tls_prf.c;': 'path = src/crypto/tls_prf.c;',
        'path = mini_aes.c;': 'path = src/crypto/aes_simple.c;',  # 合并到aes_simple.c
        'path = mini_tls.h;': 'path = src/network/mini_tls.h;',
    }
    
    # 统计更新
    updates = 0
    new_content = content
    
    for old_path, new_path in path_mapping.items():
        if old_path in new_content:
            new_content = new_content.replace(old_path, new_path)
            updates += 1
            print(f"✅ 更新: {old_path.strip()} -> {new_path.strip()}")
    
    # 写入更新后的文件
    with open(project_file, 'w', encoding='utf-8') as f:
        f.write(new_content)
    
    print("")
    print(f"📊 更新统计: {updates} 个文件路径已更新")
    print(f"💾 原文件已备份到: {backup_file}")
    print("")
    print("🎯 下一步:")
    print("  1. 在Xcode中重新打开项目")
    print("  2. 检查文件是否不再显示红色")
    print("  3. 尝试编译项目验证")
    
    return updates

def check_file_existence():
    """检查文件是否存在"""
    print("")
    print("📁 检查文件是否存在:")
    print("-" * 40)
    
    files_to_check = [
        "src/main/main.c",
        "src/core/hex_editor.c",
        "src/core/hex_editor.h",
        "src/ai/ai_interface.c",
        "src/ai/ai_interface.h",
        "src/ai/ai_integration.c",
        "src/ai/ai_integration.h",
        "src/main/ai_main.c",
        "src/main/ai_main_new.c",
        "src/ai/ai_tcp.c",
        "src/ai/ai_proxy.c",
        "src/crypto/aes_simple.c",
        "src/crypto/sha256_impl.c",
        "src/network/simple_tls.c",
        "src/crypto/tls_prf.c",
        "src/network/mini_tls.h",
    ]
    
    missing_files = []
    for file in files_to_check:
        if os.path.exists(file):
            print(f"✅ {file}")
        else:
            print(f"❌ {file} (不存在)")
            missing_files.append(file)
    
    return missing_files

def main():
    print("🚀 Xcode项目修复工具")
    print("=" * 50)
    
    # 检查文件
    missing_files = check_file_existence()
    
    if missing_files:
        print("")
        print("⚠️  警告: 以下文件不存在:")
        for file in missing_files:
            print(f"  - {file}")
        print("")
        response = input("是否继续修复? (y/n): ")
        if response.lower() != 'y':
            print("❌ 用户取消操作")
            return
    
    # 更新项目文件
    updates = update_xcode_project()
    
    if updates > 0:
        print("")
        print("🎉 Xcode项目修复完成!")
        print("请重新打开Xcode项目查看效果。")
    else:
        print("")
        print("ℹ️  未找到需要更新的文件路径")
        print("可能文件路径已经是正确的。")

if __name__ == "__main__":
    main()