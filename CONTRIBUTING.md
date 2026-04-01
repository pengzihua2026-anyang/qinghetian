# 贡献指南

感谢你考虑为AI二进制编辑器项目做出贡献！我们欢迎所有形式的贡献，包括但不限于：

- 🐛 报告Bug
- ✨ 提出新功能建议
- 📝 改进文档
- 🔧 提交代码修复
- 🎨 改进代码风格
- 🧪 添加测试用例

## 🚀 开始贡献

### 1.  Fork 仓库

1. 访问 https://github.com/zihua/ai-binary-editor
2. 点击右上角的 "Fork" 按钮
3. 克隆你的Fork到本地：
   ```bash
   git clone https://github.com/你的用户名/ai-binary-editor.git
   cd ai-binary-editor
   ```

### 2. 设置开发环境

```bash
# 安装编译工具
# macOS
brew install clang make

# Ubuntu/Debian
sudo apt-get install clang make

# 编译项目
make ai
```

### 3. 创建分支

```bash
git checkout -b feature/你的功能名称
# 或
git checkout -b fix/修复的问题描述
```

### 4. 进行修改

按照项目规范进行代码修改。

### 5. 提交更改

```bash
# 添加修改的文件
git add .

# 提交更改（使用语义化提交信息）
git commit -m "feat: 添加新功能描述"
# 或
git commit -m "fix: 修复问题描述"
# 或
git commit -m "docs: 更新文档"
```

### 6. 推送到GitHub

```bash
git push origin feature/你的功能名称
```

### 7. 创建Pull Request

1. 访问你的Fork仓库页面
2. 点击 "Compare & pull request"
3. 填写PR描述，说明你的修改
4. 提交PR等待审核

## 📋 贡献规范

### 代码规范

#### 语言标准
- 使用 **C11** 标准
- 避免使用平台特定扩展

#### 代码风格
- 使用 **4个空格** 缩进（非Tab）
- 函数和变量使用 **snake_case**
- 常量使用 **UPPER_CASE**
- 结构体类型使用 **CamelCase**

#### 函数规范
```c
// 函数前必须有详细注释
/**
 * 函数功能描述
 * 
 * @param param1 参数1描述
 * @param param2 参数2描述
 * @return 返回值描述
 * @note 注意事项
 */
return_type function_name(type param1, type param2) {
    // 函数体
}
```

#### 错误处理
```c
// 所有可能失败的操作都必须检查返回值
if (function_call() != SUCCESS) {
    // 详细的错误处理
    fprintf(stderr, "错误描述: %s\n", strerror(errno));
    return ERROR_CODE;
}
```

#### 内存管理
```c
// 分配内存必须检查
void *ptr = malloc(size);
if (!ptr) {
    fprintf(stderr, "内存分配失败\n");
    return NULL;
}

// 使用后必须释放
free(ptr);
ptr = NULL;  // 避免悬空指针
```

### 提交信息规范

使用[语义化提交信息](https://www.conventionalcommits.org/)：

```
类型(范围): 描述

正文（可选）

脚注（可选）
```

#### 类型说明：
- **feat**: 新功能
- **fix**: 修复Bug
- **docs**: 文档更新
- **style**: 代码格式调整
- **refactor**: 代码重构
- **test**: 测试相关
- **chore**: 构建过程或辅助工具

#### 示例：
```
feat(network): 添加TLS握手实现

- 实现Client Hello/Server Hello协议
- 添加证书验证基础框架
- 更新网络测试用例

Closes #123
```

### 文档规范

#### 代码注释
- 每个文件开头必须有文件描述
- 每个函数必须有详细注释
- 复杂算法必须有算法说明
- 关键代码必须有行内注释

#### Markdown文档
- 使用中文编写（技术术语可用英文）
- 结构清晰，层次分明
- 代码示例要完整可运行
- 图片和图表要有描述

## 🧪 测试要求

### 单元测试
- 新功能必须包含单元测试
- 修复Bug必须添加回归测试
- 测试覆盖率不应降低

### 集成测试
- 网络功能必须测试实际连接
- AI功能必须测试真实API
- 二进制编辑必须测试各种文件格式

### 测试运行
```bash
# 运行所有测试
make test

# 运行AI版本测试
make ai-test

# 运行特定测试
./test_specific_feature
```

## 🔍 代码审查

### 审查流程
1. PR创建后会自动运行CI测试
2. 至少需要1名核心成员批准
3. 所有CI测试必须通过
4. 代码必须符合项目规范

### 审查重点
- 代码逻辑正确性
- 性能影响评估
- 安全性考虑
- 兼容性影响
- 文档完整性

## 🐛 报告Bug

### 创建Issue
1. 访问 https://github.com/zihua/ai-binary-editor/issues
2. 点击 "New issue"
3. 选择 "Bug report" 模板
4. 填写详细信息

### Bug报告模板
```
## 问题描述
清晰描述问题现象

## 重现步骤
1. 第一步
2. 第二步
3. ...

## 预期行为
应该发生什么

## 实际行为
实际发生了什么

## 环境信息
- 操作系统: 
- 编译器版本: 
- 项目版本: 
- 其他相关信息: 

## 附加信息
日志、截图、相关代码等
```

## 💡 提出新功能

### 功能建议模板
```
## 功能描述
清晰描述想要的功能

## 使用场景
在什么情况下需要这个功能

## 实现建议
如果有，提供实现思路

## 替代方案
考虑过的其他方案

## 附加信息
相关参考资料、示例等
```

## 🏆 贡献者奖励

### 贡献者名单
所有贡献者都会在项目的 [CONTRIBUTORS.md](CONTRIBUTORS.md) 文件中列出。

### 特殊贡献
- **重大功能贡献**: 在发布说明中特别感谢
- **安全漏洞报告**: 在安全公告中致谢
- **文档重要改进**: 在文档页脚致谢

## ❓ 常见问题

### Q: 我可以从哪里开始贡献？
A: 查看 [Good First Issues](https://github.com/zihua/ai-binary-editor/issues?q=is%3Aissue+is%3Aopen+label%3A%22good+first+issue%22) 标签。

### Q: 如何获取帮助？
A: 可以在Issue中提问，或通过电子邮件联系维护者。

### Q: 贡献有大小限制吗？
A: 没有，小到错别字修复，大到新功能实现，我们都欢迎！

### Q: 我的PR多久会被审查？
A: 我们会在3个工作日内进行初步审查。

## 📞 联系方式

- 项目维护者: 子华 (zihua@example.com)
- GitHub Issues: [问题反馈](https://github.com/zihua/ai-binary-editor/issues)
- 讨论区: [GitHub Discussions](https://github.com/zihua/ai-binary-editor/discussions)

---

感谢你的贡献！你的每一行代码、每一个建议都在帮助这个项目变得更好。🎉

**一起为AI时代打造完全自主的底层工具！**