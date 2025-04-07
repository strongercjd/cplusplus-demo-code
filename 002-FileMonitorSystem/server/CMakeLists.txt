cmake_minimum_required(VERSION 3.10)
project(IPC_Server)

set(CMAKE_CXX_STANDARD 17)

# 在server子项目中添加
add_definitions(-DPROJECT_ROOT="${CMAKE_SOURCE_DIR}")


# 可执行文件配置
add_executable(ipc_server
    src/main.cpp
    src/IPCManager.cpp
    src/FileWatcher.cpp
    src/SubscriptionManager.cpp
    src/AuthManager.cpp
)

target_include_directories(ipc_server PRIVATE
    ${CMAKE_SOURCE_DIR}/server/include         # 服务端专属头文件
    ${CMAKE_SOURCE_DIR}/common/include         # 公共协议头文件
    ${CMAKE_SOURCE_DIR}/common/include/nlohmann # JSON头文件
)

target_link_libraries(ipc_server
    pthread
)
