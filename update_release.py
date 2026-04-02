#!/usr/bin/env python3
"""
更新GitHub Release内容
"""

import os
import json
import subprocess
import sys

def run_command(cmd):
    """运行命令并返回输出"""
    try:
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
        return result.stdout.strip(), result.stderr.strip(), result.returncode
    except Exception as e:
        return "", str(e), 1

def get_latest_release_id():
    """获取最新Release的ID"""
    token = os.environ.get('GITHUB_TOKEN')
    if not token:
        return None
    
    cmd = f'''curl -s -H "Authorization: token {token}" \
  -H "Accept: application/vnd.github.v3+json" \
  https://api.github.com/repos/pengzihua2026-anyang/ai-binary-editor/releases/latest'''
    
    stdout, stderr, code = run_command(cmd)
    if code == 0 and stdout:
        try:
            data = json.loads(stdout)
            return data.get('id')
        except:
            return None
    return None

def update_release_content():
    """更新Release内容"""
    print("🔄 更新GitHub Release内容")
    print("=" * 50)
    
    # 读取GitHub Token
    token = os.environ.get('GITHUB_TOKEN')
    if not token:
        print("❌ 未找到GITHUB_TOKEN环境变量")
        return 1
    
    # 获取Release ID
    release_id = get_latest_release_id()
    if not release_id:
        print("❌ 无法获取Release ID")
        return 1
    
    print(f"📋 Release ID: {release_id}")
    
    # 读取双语发布内容
    with open('RELEASE_BILINGUAL.md', 'r', encoding='utf-8') as f:
        release_body = f.read()
    
    # 创建更新数据
    update_data = {
        "body": release_body,
        "name": "AI Binary Editor v1.0 (Bilingual)"
    }
    
    # 保存为临时文件
    with open('update_data.json', 'w', encoding='utf-8') as f:
        json.dump(update_data, f, ensure_ascii=False, indent=2)
    
    print("📝 更新数据已保存到 update_data.json")
    print("")
    print("📋 新内容预览:")
    print("-" * 40)
    print(release_body[:500] + "...")
    print("-" * 40)
    
    # 更新Release
    cmd = f'''curl -X PATCH \
  -H "Authorization: token {token}" \
  -H "Accept: application/vnd.github.v3+json" \
  https://api.github.com/repos/pengzihua2026-anyang/ai-binary-editor/releases/{release_id} \
  -d @update_data.json'''
    
    print("")
    print("📤 正在更新GitHub Release...")
    stdout, stderr, code = run_command(cmd)
    
    if code == 0:
        try:
            result = json.loads(stdout)
            print("✅ GitHub Release更新成功！")
            print("")
            print("📊 更新后信息:")
            print(f"   名称: {result.get('name', 'N/A')}")
            print(f"   标签: {result.get('tag_name', 'N/A')}")
            print(f"   URL: {result.get('html_url', 'N/A')}")
            print(f"   更新时间: {result.get('updated_at', 'N/A')}")
        except json.JSONDecodeError:
            print("⚠️  响应不是有效的JSON")
            print("响应:", stdout[:200])
    else:
        print("❌ 更新Release失败")
        print("错误:", stderr)
        print("输出:", stdout[:200])
    
    # 清理临时文件
    if os.path.exists('update_data.json'):
        os.remove('update_data.json')
    
    return 0

def main():
    print("🚀 AI二进制编辑器 - 更新发布内容")
    print("=" * 50)
    
    # 检查当前Release
    token = os.environ.get('GITHUB_TOKEN')
    if not token:
        print("❌ 未找到GITHUB_TOKEN环境变量")
        return 1
    
    print("1. 检查当前Release状态...")
    cmd = f'''curl -s -H "Authorization: token {token}" \
  -H "Accept: application/vnd.github.v3+json" \
  https://api.github.com/repos/pengzihua2026-anyang/ai-binary-editor/releases/latest'''
    
    stdout, stderr, code = run_command(cmd)
    if code == 0 and stdout:
        try:
            data = json.loads(stdout)
            print(f"✅ 找到Release: {data.get('name')}")
            print(f"   标签: {data.get('tag_name')}")
            print(f"   URL: {data.get('html_url')}")
            print(f"   创建时间: {data.get('created_at')}")
        except:
            print("❌ 无法解析Release信息")
    else:
        print("❌ 无法获取Release信息")
    
    print("")
    print("2. 更新Release内容...")
    return update_release_content()

if __name__ == "__main__":
    sys.exit(main())