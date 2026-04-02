#!/usr/bin/env python3
"""
创建GitHub Release
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

def main():
    print("🚀 创建GitHub Release")
    print("=" * 50)
    
    # 读取GitHub Token
    token = os.environ.get('GITHUB_TOKEN')
    if not token:
        print("❌ 未找到GITHUB_TOKEN环境变量")
        return 1
    
    # 读取简化Release内容
    with open('simple_release.md', 'r', encoding='utf-8') as f:
        release_body = f.read()
    
    # 创建JSON数据
    release_data = {
        "tag_name": "v1.0",
        "name": "AI Binary Editor v1.0",
        "body": release_body,
        "draft": False,
        "prerelease": False
    }
    
    # 保存为临时文件
    with open('release_data.json', 'w', encoding='utf-8') as f:
        json.dump(release_data, f, ensure_ascii=False, indent=2)
    
    print("📝 Release数据已保存到 release_data.json")
    print("")
    print("📋 Release内容预览:")
    print("-" * 40)
    print(release_body[:500] + "...")
    print("-" * 40)
    
    # 使用curl创建Release
    cmd = f'''curl -X POST \
  -H "Authorization: token {token}" \
  -H "Accept: application/vnd.github.v3+json" \
  https://api.github.com/repos/pengzihua2026-anyang/ai-binary-editor/releases \
  -d @release_data.json'''
    
    print("")
    print("📤 正在创建GitHub Release...")
    stdout, stderr, code = run_command(cmd)
    
    if code == 0:
        try:
            result = json.loads(stdout)
            print("✅ GitHub Release创建成功！")
            print("")
            print("📊 Release信息:")
            print(f"   名称: {result.get('name', 'N/A')}")
            print(f"   标签: {result.get('tag_name', 'N/A')}")
            print(f"   URL: {result.get('html_url', 'N/A')}")
            print(f"   发布时间: {result.get('created_at', 'N/A')}")
        except json.JSONDecodeError:
            print("⚠️  响应不是有效的JSON")
            print("响应:", stdout[:200])
    else:
        print("❌ 创建Release失败")
        print("错误:", stderr)
        print("输出:", stdout[:200])
    
    # 清理临时文件
    if os.path.exists('release_data.json'):
        os.remove('release_data.json')
    
    return 0

if __name__ == "__main__":
    sys.exit(main())