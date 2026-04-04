# 青禾田项目网站

这是青禾田(QingHeTian)项目的官方网站，使用GitHub Pages构建。

## 网站结构

```
docs/
├── _config.yml          # Jekyll配置文件
├── _layouts/            # 布局模板
│   └── default.html    # 默认布局
├── _posts/             # 博客文章
│   ├── 2026-04-02-project-launch.md
│   └── 2026-04-02-technical-architecture.md
├── assets/             # 静态资源
├── index.html          # 首页
└── README.md           # 本文件
```

## 本地开发

### 安装依赖
```bash
cd docs
bundle install
```

### 本地运行
```bash
bundle exec jekyll serve
```

访问 http://localhost:4000/QingHeTian-BinaryEditor/ 查看网站

## 部署到GitHub Pages

### 方法1：自动部署
1. 将docs文件夹推送到GitHub
2. 访问仓库 Settings → Pages
3. 选择 Source: Deploy from a branch
4. 选择 Branch: main, Folder: /docs
5. 点击 Save

### 方法2：手动部署
```bash
cd docs
bundle exec jekyll build
# _site目录包含生成的静态网站
```

## 添加新文章

1. 在 `_posts/` 目录创建Markdown文件
2. 文件名格式: `YYYY-MM-DD-title.md`
3. 文件头部添加YAML front matter:
```yaml
---
layout: post
title: "文章标题"
date: YYYY-MM-DD HH:MM:SS +0800
categories: [分类1, 分类2]
author: 作者名
---
```

## 自定义样式

修改 `_layouts/default.html` 中的CSS样式，或创建单独的CSS文件放在 `assets/` 目录。

## 多语言支持

网站采用双语格式：
- 中文内容在前
- 英文内容在后
- 用分隔线隔开

## 许可证

网站内容遵循与项目相同的许可证：
- 开源许可证: GPL v3.0
- 商业授权: 规划中

## 联系方式

- 项目主页: https://github.com/pengzihua2026-anyang/qinghetian
- 网站地址: https://pengzihua2026-anyang.github.io/QingHeTian-BinaryEditor/
- 作者邮箱: pengzihua2026@163.com