cmake_minimum_required(VERSION 3.10)
project(IPC_Client)

set(CMAKE_CXX_STANDARD 17)


add_executable(ipc_client
    src/main.cpp
    src/ClientHandler.cpp
)

target_include_directories(ipc_client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include        # 客户端专属头文件
    ${CMAKE_SOURCE_DIR}/common/include         # 公共协议头文件
    ${CMAKE_SOURCE_DIR}/common/include/nlohmann # JSON头文件路径
)

target_link_libraries(ipc_client
    pthread
)
