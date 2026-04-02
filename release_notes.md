# 🚀 AI Binary Editor v1.0 Release

## 🇨🇳 中文发布说明

$(cat RELEASE_CN.md)

---

## 🇺🇸 English Release Notes

$(cat RELEASE_EN.md)
EOF && echo "✅ Release内容已准备" && echo "" && echo "创建GitHub Release..." && curl -X POST -H "Authorization: token $GITHUB_TOKEN" -H "Accept: application/vnd.github.v3+json" https://api.github.com/repos/pengzihua2026-anyang/ai-binary-editor/releases -d '{"tag_name":"v1.0","name":"AI Binary Editor v1.0","body":"'"$(cat release_notes.md | sed 's/"/\\"/g' | tr '\n' ' ')"'","draft":false,"prerelease":false}' 2>&1 | head -30