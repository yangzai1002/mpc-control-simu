CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = log_config_generator
SOURCE = log_config_generator.cpp

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET)

# 运行示例，使用当前目录作为path1和gea/runtime_service作为path2
run: $(TARGET)
	./$(TARGET) . ./gea/runtime_service

# 帮助信息
help:
	@echo "Available targets:"
	@echo "  make         - 编译程序"
	@echo "  make clean   - 清理编译文件"
	@echo "  make run     - 编译并运行示例"
	@echo "  make help    - 显示此帮助信息"
