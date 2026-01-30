cmake_minimum_required(VERSION 3.10)

# 项目名称
project(thread_msg_queue)

set(CMAKE_C_STANDARD 99) 
set(CMAKE_C_STANDARD_REQUIRED ON)

# 添加可执行文件
add_executable(${PROJECT_NAME} thread_msg_queue.c)

# 链接线程库 pthread
target_link_libraries(${PROJECT_NAME} pthread)
