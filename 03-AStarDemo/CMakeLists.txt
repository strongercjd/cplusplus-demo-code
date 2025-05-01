# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)#  指定cmake版本
project(AStarDemo LANGUAGES CXX) # 项目名称 C++语言

set(CMAKE_CXX_STANDARD 11) # 指定c++版本
set(CMAKE_CXX_STANDARD_REQUIRED ON)# 强制使用指定的c++版本

find_package(OpenCV 4.2.0 REQUIRED) # 查找OpenCV库
include_directories(${OpenCV_INCLUDE_DIRS}) # 包含OpenCV头文件目录

# 头文件的路径
include_directories(
    include
)

add_executable(AStarDemo # 添加可执行文件
main.cpp
src/AStar.cpp
src/OpenCVProcessor.cpp
) 
target_link_libraries(AStarDemo ${OpenCV_LIBS}) # 链接OpenCV库

