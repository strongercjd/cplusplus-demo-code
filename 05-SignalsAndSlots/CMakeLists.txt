# 设置CMake的版本要求
cmake_minimum_required(VERSION 2.8)

# 设置项目名称
project(SignalsAndSlots)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)

# 添加头文件搜索路径
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 添加可执行文件
add_executable(SignalsAndSlots main.cpp)
