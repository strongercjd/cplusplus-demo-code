cmake_minimum_required(VERSION 3.14)
project(test_new CXX)

set(CMAKE_CXX_STANDARD 11)

set(TEST_NEW_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# 编译选项（仅 header 的不用 -g 也可，为了一致性都加）
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_COMPILER_IS_GNUCXX)
  set(COMMON_FLAGS -Wall -Wextra -g)
endif()

# rectrace_main
add_executable(rectrace_main rectrace_main.cpp backward.cpp)
# 添加 libdw 库
target_link_libraries(rectrace_main PRIVATE dw)
# 定义 BACKWARD_HAS_DW=1 宏
target_compile_definitions(rectrace_main PRIVATE BACKWARD_HAS_DW=1)
target_include_directories(rectrace_main PRIVATE ${TEST_NEW_DIR})
# 添加 common 编译选项
if(COMMON_FLAGS)
  target_compile_options(rectrace_main PRIVATE ${COMMON_FLAGS})
endif()

# select_signals_main
add_executable(select_signals_main select_signals_main.cpp backward.cpp)
# 添加 libdw 库
target_link_libraries(select_signals_main PRIVATE dw)
# 定义 BACKWARD_HAS_DW=1 宏
target_compile_definitions(select_signals_main PRIVATE BACKWARD_HAS_DW=1)
target_include_directories(select_signals_main PRIVATE ${TEST_NEW_DIR})
# 添加 common 编译选项
if(COMMON_FLAGS)
  target_compile_options(select_signals_main PRIVATE ${COMMON_FLAGS})
endif()

# stacktrace_main
add_executable(stacktrace_main stacktrace_main.cpp backward.cpp)
# 添加 libdw 库
target_link_libraries(stacktrace_main PRIVATE dw)
# 定义 BACKWARD_HAS_DW=1 宏
target_compile_definitions(stacktrace_main PRIVATE BACKWARD_HAS_DW=1)
target_include_directories(stacktrace_main PRIVATE ${TEST_NEW_DIR})
# 添加 common 编译选项
if(COMMON_FLAGS)
  target_compile_options(stacktrace_main PRIVATE ${COMMON_FLAGS})
endif()

# suicide_main
add_executable(suicide_main suicide_main.cpp backward.cpp)
# 添加 libdw 库
target_link_libraries(suicide_main PRIVATE dw)
# 定义 BACKWARD_HAS_DW=1 宏
target_compile_definitions(suicide_main PRIVATE BACKWARD_HAS_DW=1)
target_include_directories(suicide_main PRIVATE ${TEST_NEW_DIR})
# 添加 common 编译选项
if(COMMON_FLAGS)
  target_compile_options(suicide_main PRIVATE ${COMMON_FLAGS})
endif()
