cmake_minimum_required(VERSION 3.10)

# 项目名称
project(TimerApp)

# 设置 C++ 标准（至少需要 C++11，因为使用了 std::thread, std::atomic 等特性）
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加可执行文件
add_executable(${PROJECT_NAME} main.cpp timer.cpp timer.hpp)

# 如果需要链接线程库（某些系统需要）
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} Threads::Threads)
