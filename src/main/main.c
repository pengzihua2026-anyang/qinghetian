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

