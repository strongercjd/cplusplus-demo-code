cmake_minimum_required(VERSION 3.14)
project(test_new CXX)

set(CMAKE_CXX_STANDARD 11)

set(TEST_NEW_DIR ${CMAKE_CURRENT_SOURCE_DIR})

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

set(BACKWARD_APPS rectrace_main select_signals_main stacktrace_main suicide_main)

foreach(app IN LISTS BACKWARD_APPS)
  add_executable(${app} ${app}.cpp backward.cpp)
  # 添加 libdw 库
  target_link_libraries(${app} PRIVATE dw)
  # 定义 BACKWARD_HAS_DW=1 宏
  target_compile_definitions(${app} PRIVATE BACKWARD_HAS_DW=1)
  # 添加 include 目录
  target_include_directories(${app} PRIVATE ${TEST_NEW_DIR})
  # 添加 common 编译选项
  if(COMMON_FLAGS)
    target_compile_options(${app} PRIVATE ${COMMON_FLAGS})
  endif()
endforeach()
