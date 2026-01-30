#include "sysv_msg_common.h"
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>

int main()
{
    int32_t msg_queue_id = -1;
    const int32_t recv_msg_count = 10; // 计划接收 10 条消息（与生产者对应）

    // 1. 获取 System V 消息队列（与生产者使用相同键值）
    msg_queue_id = msgget(MSG_QUEUE_KEY, 0666); // 仅获取，不创建（依赖生产者创建或已存在）
    if (msg_queue_id == -1)
    {
        perror("msgget failed (获取消息队列失败，可能队列尚未创建)");
        return EXIT_FAILURE;
    }
    std::cout << "消费者：成功获取消息队列，队列 ID：" << msg_queue_id << std::endl;

    // 2. 循环接收消息
    SysVMessage msg;
    for (int32_t i = 0; i < recv_msg_count; ++i)
    {
        // 从消息队列接收消息
        // msgrcv 参数：队列 ID、消息指针、消息正文长度、消息类型、标志位（0：阻塞接收，队列空则等待）
        ssize_t ret = msgrcv(msg_queue_id, &msg, sizeof(msg.msg_content), MSG_TYPE, 0);
        if (ret == -1)
        {
            perror("msgrcv failed (接收消息失败)");
            // 清理消息队列资源
            msgctl(msg_queue_id, IPC_RMID, nullptr);
            return EXIT_FAILURE;
        }

        std::cout << "消费者：接收消息成功 | 内容：" << msg.msg_content << std::endl;

        // 模拟消费耗时（每 1 秒接收一条，便于观察效果）
        usleep(1000000);
    }

    // 3. 接收完毕，删除 System V 消息队列（内核资源不会自动释放，必须手动清理）
    int32_t ctl_ret = msgctl(msg_queue_id, IPC_RMID, nullptr);
    if (ctl_ret == -1)
    {
        perror("msgctl failed (删除消息队列失败)");
        return EXIT_FAILURE;
    }
    std::cout << "消费者：所有消息接收完毕，已删除消息队列，进程退出" << std::endl;

    return EXIT_SUCCESS;
}