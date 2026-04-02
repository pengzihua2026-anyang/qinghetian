#!/bin/bash

# GPL v3.0 版权头模板
GPL_HEADER_C="/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 安阳子涵软件开发有限公司
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */"

echo "正在为二进制编辑器项目添加GPL版权头..."

# 处理所有C文件
for file in $(find src -name "*.c"); do
    echo "处理: $file"
    # 检查是否已经有版权头
    if ! grep -q "SPDX-License-Identifier" "$file"; then
        # 备份原文件
        cp "$file" "$file.bak"
        # 添加版权头
        echo "$GPL_HEADER_C" > "$file.tmp"
        cat "$file" >> "$file.tmp"
        mv "$file.tmp" "$file"
        echo "  ✓ 已添加版权头"
    else
        echo "  ⏭️ 已有版权头，跳过"
    fi
done

# 处理所有头文件
for file in $(find src -name "*.h"); do
    echo "处理: $file"
    if ! grep -q "SPDX-License-Identifier" "$file"; then
        cp "$file" "$file.bak"
        echo "$GPL_HEADER_C" > "$file.tmp"
        cat "$file" >> "$file.tmp"
        mv "$file.tmp" "$file"
        echo "  ✓ 已添加版权头"
    else
        echo "  ⏭️ 已有版权头，跳过"
    fi
done

echo "完成！所有源文件已添加GPL v3.0版权头。"