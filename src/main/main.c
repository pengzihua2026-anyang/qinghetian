/*
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
 */
//
//  main.c
//  binary
//
//  Created by Zihua on 2026/4/1.
//  重构为AI专用命令行接口
//

#include <stdio.h>
#include <stdlib.h>
#include "ai_interface.h"

int main(int argc, const char *argv[]) {
    // 解析命令行参数
    CommandArgs *args = parse_args(argc, argv);
    if (!args) {
        print_error("参数解析失败");
        return EXIT_INVALID_ARGS;
    }
    
    // 执行命令
    int result = execute_command(args);
    
    // 清理
    free_args(args);
    
    return result;
}

