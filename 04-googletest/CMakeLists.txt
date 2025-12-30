cmake_minimum_required(VERSION 3.9)
project(MathUtilsTest)

# 设置C++标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

link_directories(  
  ${CMAKE_SOURCE_DIR}/test_libs/lib/x86
)

# 添加头文件搜索路径
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/test_libs/inc
)


# 创建测试可执行文件
add_executable(
  math_demo
  src/math_demo.cpp
  src/math_utils.cpp
)

# 创建测试可执行文件（原有简单测试）
add_executable(
  math_utils_test_demo
  test/math_utils_test.cpp
  src/math_utils.cpp
)

# 创建全面的gtest演示测试可执行文件
add_executable(
  gtest_demo
  test/gtest_demo.cpp
  src/math_utils.cpp
  src/test_utils.cpp
)

# 链接gtest库（使用绝对路径）
target_link_libraries(
  math_utils_test_demo
  ${CMAKE_SOURCE_DIR}/test_libs/lib/x86/libgtest_main.a
  ${CMAKE_SOURCE_DIR}/test_libs/lib/x86/libgtest.a
  pthread
)

# 链接gtest库到全面演示测试
target_link_libraries(
  gtest_demo
  ${CMAKE_SOURCE_DIR}/test_libs/lib/x86/libgtest_main.a
  ${CMAKE_SOURCE_DIR}/test_libs/lib/x86/libgtest.a
  pthread
)



# </start>注册可执行文件，这样就可以通过ctest --verbose 来执行单元测试
# 启用测试
enable_testing()

# 手动注册测试（CMake 2.8 兼容的方式）
# add_test(NAME MathUtilsTest COMMAND math_utils_test_demo)
# add_test(NAME GTestDemo COMMAND gtest_demo)

# 注意：以下功能需要 CMake 3.9+，CMake 2.8 不支持
include(GoogleTest)
gtest_discover_tests(math_utils_test_demo)
gtest_discover_tests(gtest_demo)

# </end>注册可执行文件，这样就可以通过ctest --verbose 来执行单元测试
