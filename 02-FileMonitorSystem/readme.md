# 文件监控系统 (File Monitor System)

一个基于 Unix Domain Socket 的进程间通信（IPC）框架，用于实时监控文件变化并向客户端推送更新通知。

## 项目简介

本项目实现了一个高性能的文件监控系统，支持：
- 实时文件监控（基于 Linux inotify）
- 客户端订阅/取消订阅机制
- 用户认证系统
- 基于 JSON 的消息协议
- 多客户端并发支持

## 项目结构

```
02-FileMonitorSystem/
├── CMakeLists.txt          # 根目录构建配置
├── server/                 # 服务端代码
│   ├── CMakeLists.txt
│   ├── include/           # 服务端头文件
│   │   ├── AuthManager.hpp      # 认证管理器
│   │   ├── FileWatcher.hpp      # 文件监控器
│   │   └── IPCManager.hpp       # IPC 管理器
│   ├── src/               # 服务端源文件
│   │   ├── main.cpp
│   │   ├── AuthManager.cpp
│   │   ├── FileWatcher.cpp
│   │   ├── IPCManager.cpp
│   │   └── SubscriptionManager.cpp
│   └── configs/           # 配置文件目录
│       └── app.json
├── client/                # 客户端代码
│   ├── CMakeLists.txt
│   ├── include/
│   │   └── ClientHandler.hpp    # 客户端处理器
│   └── src/
│       ├── main.cpp
│       └── ClientHandler.cpp
└── common/                # 公共代码
    └── include/
        ├── Protocol.hpp          # 通信协议定义
        └── nlohmann/
            └── json.hpp          # JSON 库
```

## 功能特性

### 服务端功能
- **文件监控**：使用 Linux inotify 机制实时监控文件变化
- **IPC 管理**：基于 Unix Domain Socket 的进程间通信
- **订阅管理**：管理客户端对文件的订阅关系
- **用户认证**：支持用户名密码认证机制
- **多客户端支持**：支持多个客户端同时连接和订阅

### 客户端功能
- **文件订阅**：订阅指定文件的变化通知
- **实时接收**：异步接收文件更新通知
- **优雅退出**：支持信号处理，优雅关闭连接

## 系统要求

- **操作系统**：Linux（需要 inotify 支持）
- **编译器**：支持 C++17 的编译器（如 GCC 7+ 或 Clang 5+）
- **构建工具**：CMake 3.10 或更高版本
- **依赖库**：
  - pthread（POSIX 线程库）
  - nlohmann/json（头文件库，已包含在项目中）

## 编译与安装

### 1. 克隆或进入项目目录

```bash
cd 02-FileMonitorSystem
```

### 2. 创建构建目录

```bash
mkdir build
cd build
```

### 3. 配置和编译

```bash
cmake ..
make
```

### 4. 编译产物

编译完成后，在 `build/server/` 和 `build/client/` 目录下会生成可执行文件：
- `ipc_server`：服务端程序
- `ipc_client`：客户端程序

## 使用方法

### 启动服务端

```bash
./build/server/ipc_server
```

服务端启动后会：
1. 创建 Unix Domain Socket：`/tmp/ipc_socket.sock`
2. 开始监控配置文件：`server/configs/app.json`
3. 等待客户端连接

### 运行客户端

在另一个终端中运行客户端，订阅指定文件：

```bash
./build/client/ipc_client <文件名>
```

例如，订阅 `app.json` 文件：

```bash
./build/client/ipc_client server/configs/app.json
```

### 测试文件监控

修改被监控的文件（如 `server/configs/app.json`），客户端会立即收到更新通知。

### 停止程序

- 服务端/客户端：按 `Ctrl+C` 或发送 `SIGTERM` 信号
- 程序会优雅关闭，清理资源

## 通信协议

系统使用基于 JSON 的消息协议，定义在 `common/include/Protocol.hpp` 中。

### 消息类型

- `SUBSCRIBE`：订阅文件更新
- `UNSUBSCRIBE`：取消订阅
- `FILE_UPDATE`：文件更新通知
- `SUBSCRIBE_RESPONSE`：订阅响应
- `AUTH_REQUEST`：认证请求
- `AUTH_RESPONSE`：认证响应
- `ERROR`：错误消息

### 消息格式

所有消息都遵循以下结构：

```json
{
  "type": "消息类型",
  "payload": {
    // 消息负载数据
  }
}
```

## 核心组件说明

### FileWatcher
- 使用 Linux inotify API 监控文件系统事件
- 支持添加多个文件监控
- 线程安全设计

### IPCManager
- 管理 Unix Domain Socket 服务器
- 处理客户端连接和消息路由
- 集成认证和订阅管理

### AuthManager
- 管理用户凭据
- 支持密码哈希存储
- 验证客户端身份

### SubscriptionManager
- 维护文件与客户端的订阅关系
- 支持一对多订阅（一个文件可被多个客户端订阅）

### ClientHandler
- 客户端连接管理
- 消息发送和接收
- 异步更新通知处理

## 注意事项

1. **Socket 路径**：默认使用 `/tmp/ipc_socket.sock`，确保有写入权限
2. **文件路径**：客户端订阅时使用相对于项目根目录的路径
3. **权限要求**：需要读取被监控文件的权限
4. **平台限制**：仅支持 Linux 系统（依赖 inotify）
