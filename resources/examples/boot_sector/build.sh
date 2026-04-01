#!/bin/bash
# 引导扇区构建脚本

echo "=== 引导扇区构建工具 ==="
echo ""

# 检查依赖
check_dependency() {
    if ! command -v $1 &> /dev/null; then
        echo "❌ 缺少依赖: $1"
        echo "请安装: $2"
        exit 1
    fi
}

check_dependency "nasm" "NASM汇编器 (brew install nasm)"
check_dependency "qemu-system-x86_64" "QEMU模拟器 (brew install qemu)"

# 清理旧文件
clean() {
    echo "清理旧文件..."
    rm -f boot.bin boot.img
}

# 编译引导扇区
compile() {
    echo "编译引导扇区..."
    nasm -f bin hello_world.asm -o boot.bin
    
    # 检查大小
    size=$(wc -c < boot.bin)
    if [ $size -ne 512 ]; then
        echo "❌ 错误: 引导扇区大小应为512字节，实际为${size}字节"
        exit 1
    fi
    
    echo "✅ 编译成功: boot.bin (${size} 字节)"
    
    # 检查引导签名
    hexdump -C boot.bin | tail -2
    echo ""
}

# 创建磁盘镜像
create_image() {
    echo "创建磁盘镜像..."
    
    # 创建1.44MB软盘镜像
    dd if=/dev/zero of=boot.img bs=512 count=2880 2>/dev/null
    dd if=boot.bin of=boot.img conv=notrunc 2>/dev/null
    
    echo "✅ 镜像创建成功: boot.img (1.44MB)"
    echo ""
}

# 运行测试
run_test() {
    echo "在QEMU中运行测试..."
    echo "按 Ctrl+Alt+G 释放鼠标，按 Ctrl+Alt+2 进入QEMU控制台，输入 'quit' 退出"
    echo ""
    
    qemu-system-x86_64 \
        -drive format=raw,file=boot.img \
        -monitor stdio \
        -display curses
}

# 显示帮助
show_help() {
    echo "用法: $0 [选项]"
    echo ""
    echo "选项:"
    echo "  clean       清理生成的文件"
    echo "  compile     仅编译引导扇区"
    echo "  image       创建磁盘镜像"
    echo "  run         运行测试 (默认)"
    echo "  all         执行所有步骤 (clean → compile → image → run)"
    echo "  help        显示此帮助"
    echo ""
    echo "示例:"
    echo "  $0 clean      # 清理"
    echo "  $0 compile    # 编译"
    echo "  $0 run        # 运行测试"
    echo "  $0 all        # 完整流程"
}

# 主程序
case "$1" in
    "clean")
        clean
        ;;
    "compile")
        compile
        ;;
    "image")
        compile
        create_image
        ;;
    "run")
        if [ ! -f boot.img ]; then
            compile
            create_image
        fi
        run_test
        ;;
    "all")
        clean
        compile
        create_image
        run_test
        ;;
    "help"|"-h"|"--help")
        show_help
        ;;
    *)
        if [ $# -eq 0 ]; then
            # 默认运行
            if [ ! -f boot.img ]; then
                compile
                create_image
            fi
            run_test
        else
            echo "❌ 未知选项: $1"
            show_help
            exit 1
        fi
        ;;
esac

echo "=== 完成 ==="