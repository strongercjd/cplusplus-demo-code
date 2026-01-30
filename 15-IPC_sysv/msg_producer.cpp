#include "sysv_msg_common.h"
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>

int main()
{
    int32_t msg_queue_id = -1;
    const int32_t send_msg_count = 10; // 计划发送 10 条消息

    // 1. 创建/获取 System V 消息队列
    // IPC_CREAT：若队列不存在则创建；0666：队列权限（所有用户可读可写，避免权限不足）
    msg_queue_id = msgget(MSG_QUEUE_KEY, IPC_CREAT | 0666);
    if (msg_queue_id == -1)
    {
        perror("msgget failed (创建/获取消息队列失败)");
        return EXIT_FAILURE;
    }
    std::cout << "生产者：成功获取消息队列，队列 ID：" << msg_queue_id << std::endl;

    // 2. 循环发送消息
    SysVMessage msg;
    for (int32_t i = 0; i < send_msg_count; ++i)
    {
        // 构造消息内容
        snprintf(msg.msg_content, MAX_MSG_CONTENT, "这是第 %d 条 System V 测试消息", i + 1);

        // 发送消息到消息队列
        // msgsnd 参数：队列 ID、消息指针、消息正文长度、标志位（0：阻塞发送，队列满则等待）
        int32_t ret = msgsnd(msg_queue_id, &msg, sizeof(msg.msg_content), 0);
        if (ret == -1)
        {
            perror("msgsnd failed (发送消息失败)");
            // 即使发送失败，也需尝试清理消息队列资源
            msgctl(msg_queue_id, IPC_RMID, nullptr);
            return EXIT_FAILURE;
        }

        std::cout << "生产者：发送消息成功 | 内容：" << msg.msg_content << std::endl;

        // 模拟生产耗时（每 500 毫秒发送一条，便于观察效果）
        usleep(500000);
    }

    std::cout << "生产者：所有消息发送完毕，进程退出" << std::endl;
    return EXIT_SUCCESS;
}