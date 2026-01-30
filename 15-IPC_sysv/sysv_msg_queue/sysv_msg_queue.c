#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>

// ************************** 配置参数 **************************
#define MSG_KEY 0x12345678 // 消息队列键值（唯一标识，跨进程需一致）
#define MAX_MSG_CONTENT 64 // 消息内容最大长度

// ************************** 系统 V 消息结构体定义 **************************
// 注意：系统 V 消息队列要求结构体第一个成员必须是 long 类型（消息类型）
typedef struct
{
    long msg_type;                     // 消息类型（必须 > 0）
    char msg_content[MAX_MSG_CONTENT]; // 消息内容
} SysVMessage;

// ************************** 线程函数定义 **************************

/**
 * 生产者线程：发送消息到系统 V 消息队列
 */
void *sysv_producer(void *arg)
{
    int msgid = *(int *)arg;
    SysVMessage msg;
    int msg_id = 0;

    while (msg_id < 15)
    {
        // 构造消息
        msg.msg_type = 1; // 消息类型（必须 > 0）
        snprintf(msg.msg_content, MAX_MSG_CONTENT, "系统 V 消息：第 %d 条", msg_id + 1);

        // 发送消息到内核消息队列
        if (msgsnd(msgid, &msg, sizeof(msg.msg_content), 0) == -1)
        {
            perror("msgsnd failed");
            pthread_exit(NULL);
        }

        printf("系统 V 生产者：发送消息成功 | 内容：%s\n", msg.msg_content);
        usleep(500000); // 模拟生产耗时
        msg_id++;
    }

    printf("系统 V 生产者：消息发送完毕，退出线程...\n");
    pthread_exit(NULL);
}

/**
 * 消费者线程：从系统 V 消息队列接收消息
 */
void *sysv_consumer(void *arg)
{
    int msgid = *(int *)arg;
    SysVMessage msg;
    int recv_count = 0;

    while (recv_count < 15)
    {
        // 从内核消息队列接收消息（接收类型为 1 的消息） 生产者没有生产数据时，消费者会阻塞
        if (msgrcv(msgid, &msg, sizeof(msg.msg_content), 1, 0) == -1)
        {
            perror("msgrcv failed");
            pthread_exit(NULL);
        }

        printf("系统 V 消费者：接收消息成功 | 内容：%s\n", msg.msg_content);
        usleep(1000000); // 模拟消费耗时
        recv_count++;
    }

    printf("系统 V 消费者：消息接收完毕，退出线程...\n");
    pthread_exit(NULL);
}

// ************************** 主函数 **************************
int main()
{
    int msgid;
    pthread_t producer_tid, consumer_tid;

    // 1. 创建/获取系统 V 消息队列
    // IPC_CREAT：不存在则创建；0666：消息队列权限（可读可写）
    msgid = msgget(MSG_KEY, IPC_CREAT | 0666);
    if (msgid == -1)
    {
        perror("msgget failed");
        return EXIT_FAILURE;
    }

    // 2. 创建线程
    if (pthread_create(&producer_tid, NULL, sysv_producer, &msgid) != 0)
    {
        perror("pthread_create (producer) failed");
        msgctl(msgid, IPC_RMID, NULL); // 删除消息队列
        return EXIT_FAILURE;
    }
    if (pthread_create(&consumer_tid, NULL, sysv_consumer, &msgid) != 0)
    {
        perror("pthread_create (consumer) failed");
        pthread_cancel(producer_tid);
        msgctl(msgid, IPC_RMID, NULL);
        return EXIT_FAILURE;
    }

    // 3. 等待线程结束
    pthread_join(producer_tid, NULL);
    pthread_join(consumer_tid, NULL);

    // 4. 删除系统 V 消息队列（内核资源不会自动释放，必须手动删除）
    if (msgctl(msgid, IPC_RMID, NULL) == -1)
    {
        perror("msgctl (IPC_RMID) failed");
        return EXIT_FAILURE;
    }

    printf("主线程：程序正常退出\n");
    return EXIT_SUCCESS;
}