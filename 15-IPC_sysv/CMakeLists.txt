# 最低 CMake 版本要求
cmake_minimum_required(VERSION 3.10)

# 项目名称（C++ 项目）
project(SysVMsgIPC_CPP LANGUAGES CXX)

# 配置 C++ 标准（使用 C++11，兼容性好，满足演示需求）
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译选项：开启警告，便于调试（可选，提升代码健壮性）
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 1. 构建生产者可执行文件
add_executable(msg_producer
    msg_producer.cpp
)

# 2. 构建消费者可执行文件
add_executable(msg_consumer
    msg_consumer.cpp
)

# 注意：System V 消息队列接口属于系统 libc，无需额外链接第三方库，CMake 自动链接