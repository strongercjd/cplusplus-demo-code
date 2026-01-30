# 线程安全消息队列 (C 语言)

基于 POSIX 线程 (pthread) 实现的**线程安全、有界环形消息队列**，演示经典的生产者-消费者模型。

## 功能概述

- **有界环形队列**：固定容量（默认 10），避免无界增长
- **线程安全**：使用互斥锁 + 条件变量保护入队/出队
- **阻塞语义**：队列满时生产者阻塞，队列空时消费者阻塞
- **消息结构**：支持消息类型 + 定长内容（可配置）

## 数据结构

- **Message**：`msg_type`（整型）+ `msg_content`（字符串，最大长度可配置）
- **MsgQueue**：环形缓冲区 + `front`/`rear`/`count` + 互斥锁 + `cond_not_full` / `cond_not_empty`

## API 说明

| 函数 | 说明 |
|------|------|
| `msg_queue_init(MsgQueue *queue)` | 初始化队列（锁与条件变量） |
| `msg_queue_destroy(MsgQueue *queue)` | 销毁队列，释放资源 |
| `msg_queue_send(MsgQueue *queue, const Message *msg)` | 入队（生产者调用，队满则阻塞） |
| `msg_queue_recv(MsgQueue *queue, Message *msg)` | 出队（消费者调用，队空则阻塞） |

## 配置参数（源码顶部）

- `MAX_QUEUE_CAPACITY`：队列最大容量（默认 10）
- `MAX_MSG_CONTENT`：消息内容最大长度（默认 64 字节）

## 构建与运行

### 依赖

- 支持 pthread 的 C 编译器（如 gcc、clang）
- CMake >= 3.10

### Linux

```bash
mkdir build && cd build
cmake ..
make
./thread_msg_queue
```
