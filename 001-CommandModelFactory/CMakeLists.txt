cmake_minimum_required(VERSION 3.10)
project(demo)

set(CMAKE_CXX_STANDARD 17)

# 包含头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 添加可执行文件
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/model_a.cpp
    src/model_b.cpp
    src/model_c.cpp
    src/model_d.cpp
    src/model_factory.cpp
    src/testclass.cpp
)