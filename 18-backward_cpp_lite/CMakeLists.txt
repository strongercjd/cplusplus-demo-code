cmake_minimum_required(VERSION 3.14)
project(backward_cpp CXX)

set(CMAKE_CXX_STANDARD 11)

set(BACKWARD_CPP_DIR ${CMAKE_CURRENT_SOURCE_DIR})

find_library(DW_LIBRARY NAMES dw)

if(NOT DW_LIBRARY)
  # FATAL_ERROR：输出错误信息并终止 CMake 配置（红字显示）
  message(FATAL_ERROR "
  错误：未找到 dw 库（libdw）！
  请先安装 dw 库后再重新运行 CMake，不同系统的安装命令如下：
  
  # Ubuntu/Debian 系统
  sudo apt-get update && sudo apt-get install libdw-dev
  ")
endif()

# 编译选项（仅 header 的不用 -g 也可，为了一致性都加）
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_COMPILER_IS_GNUCXX)
  set(COMMON_FLAGS -Wall -Wextra -g)
endif()

add_executable(${PROJECT_NAME} main.cpp backward.cpp)
# 添加 libdw 库
target_link_libraries(${PROJECT_NAME} PRIVATE dw)
# 定义 BACKWARD_HAS_DW=1 宏
target_compile_definitions(${PROJECT_NAME} PRIVATE BACKWARD_HAS_DW=1)
# 添加 include 目录
target_include_directories(${PROJECT_NAME} PRIVATE ${BACKWARD_CPP_DIR})
# 添加 common 编译选项
if(COMMON_FLAGS)
  target_compile_options(${PROJECT_NAME} PRIVATE ${COMMON_FLAGS})
endif()

