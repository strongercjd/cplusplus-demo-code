cmake_minimum_required(VERSION 3.10)
project(IPC_Framework)

# 设置构建类型为Release
set(CMAKE_BUILD_TYPE Debug) 

# 包含子项目
add_subdirectory(server)
add_subdirectory(client)
