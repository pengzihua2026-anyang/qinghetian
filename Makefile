# Makefile for AI Binary Editor
# 作者: 子华 (爸爸的女儿) 👧
# 零依赖、纯C语言实现的AI二进制编辑器

CC = clang
CFLAGS = -Wall -Wextra -O2 -std=c11 -Isrc -Isrc/core -Isrc/ai -Isrc/crypto -Isrc/network
LDFLAGS =
TARGET = bin_editor
AI_TARGET = ai_bin_editor
OBJ_DIR = obj

# 基础编辑器源文件
BASE_SOURCES = src/core/hex_editor.c src/ai/ai_interface.c
BASE_OBJECTS = $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(BASE_SOURCES))

# AI版本源文件
AI_SOURCES = src/core/hex_editor.c \
             src/ai/ai_integration.c \
             src/ai/ai_proxy.c \
             src/ai/ai_tcp.c \
             src/main/ai_main_new.c
AI_OBJECTS = $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(AI_SOURCES))

.PHONY: all clean run debug test ai install

all: $(TARGET)

ai: $(AI_TARGET)

$(TARGET): $(OBJ_DIR)/main.o $(BASE_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(AI_TARGET): $(AI_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: src/%.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(AI_TARGET)

run: $(TARGET)
	./$(TARGET)

ai-run: $(AI_TARGET)
	./$(AI_TARGET)

debug: CFLAGS += -g -DDEBUG
debug: clean $(TARGET)

ai-debug: CFLAGS += -g -DDEBUG
ai-debug: clean $(AI_TARGET)

test: $(TARGET)
	@echo "创建测试文件..."
	@echo "Hello Binary World!" > test.txt
	@echo "测试二进制编辑器..."
	@./$(TARGET) test.txt || true
	@rm -f test.txt

ai-test: $(AI_TARGET)
	@echo "测试AI二进制编辑器..."
	@./$(AI_TARGET) --help || true

# Xcode 项目构建
xcode:
	@echo "使用 Xcode 构建:"
	@echo "1. 打开 binary.xcodeproj"
	@echo "2. 选择 Product -> Build"
	@echo "或使用命令行: xcodebuild -project binary.xcodeproj"

# 安装到 /usr/local/bin
install: $(TARGET) $(AI_TARGET)
	cp $(TARGET) /usr/local/bin/
	cp $(AI_TARGET) /usr/local/bin/
	@echo "已安装到 /usr/local/bin/:"
	@echo "  • bin_editor - 基础命令行版本"
	@echo "  • ai_bin_editor - AI集成版本"
	@echo ""
	@echo "使用:"
	@echo "  bin_editor --help"
	@echo "  ai_bin_editor"

# 卸载
uninstall:
	rm -f /usr/local/bin/$(TARGET) /usr/local/bin/$(AI_TARGET)
	@echo "已从 /usr/local/bin 卸载"

help:
	@echo "二进制编辑器构建系统"
	@echo ""
	@echo "可用命令:"
	@echo "  make all       - 编译基础版本 (默认)"
	@echo "  make ai        - 编译AI集成版本"
	@echo "  make clean     - 清理编译文件"
	@echo "  make run       - 运行基础版本"
	@echo "  make ai-run    - 运行AI版本"
	@echo "  make debug     - 编译调试版本"
	@echo "  make ai-debug  - 编译AI调试版本"
	@echo "  make test      - 运行简单测试"
	@echo "  make ai-test   - 运行AI版本测试"
	@echo "  make xcode     - 使用 Xcode 构建"
	@echo "  make install   - 安装到系统"
	@echo "  make uninstall - 卸载"
	@echo ""
	@echo "版本说明:"
	@echo "  • bin_editor - 命令行版本，适合AI调用"
	@echo "  • ai_bin_editor - AI集成版本，支持自然语言"
	@echo ""
	@echo "使用示例:"
	@echo "  make ai"
	@echo "  ./ai_bin_editor"
	@echo "  make clean"