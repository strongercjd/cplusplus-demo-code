#ifndef SYSV_MSG_COMMON_H
#define SYSV_MSG_COMMON_H

#include <cstdint>
#include <cstring>
#include <iostream>

// ************************** 系统 V 消息队列配置 **************************
#define MSG_QUEUE_KEY 0x12345678 // 消息队列唯一键值（跨进程必须一致，用于标识同一个队列）
#define MAX_MSG_CONTENT 128      // 消息内容最大长度
#define MSG_TYPE 1L              // 消息类型（必须 > 0，System V 消息队列要求）

// ************************** 系统 V 消息结构体 **************************
// 注意：System V 消息队列强制要求：结构体第一个成员必须是 long 类型（用于指定消息类型）
struct SysVMessage
{
    long msg_type;                     // 消息类型（必须 > 0，与 MSG_TYPE 对应）
    char msg_content[MAX_MSG_CONTENT]; // 消息正文内容

    // 构造函数（简化 C++ 中消息对象的初始化）
    SysVMessage() : msg_type(MSG_TYPE)
    {
        memset(msg_content, 0, sizeof(msg_content)); // 初始化内容缓冲区为 0
    }
};

#endif // SYSV_MSG_COMMON_H