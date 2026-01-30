#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// ************************** 配置参数 **************************
#define MAX_QUEUE_CAPACITY 10 // 消息队列最大容量
#define MAX_MSG_CONTENT 64    // 消息内容最大长度

// ************************** 消息结构体定义 **************************
typedef struct
{
    int msg_type;                      // 消息类型（自定义，用于区分不同消息）
    char msg_content[MAX_MSG_CONTENT]; // 消息内容
} Message;

// ************************** 消息队列结构体定义 **************************
typedef struct
{
    Message buffer[MAX_QUEUE_CAPACITY]; // 消息缓冲区（存储消息）
    int front;                          // 队列头索引（出队时使用）
    int rear;                           // 队列尾索引（入队时使用）
    int count;                          // 当前队列中的消息数量
    pthread_mutex_t mutex;              // 互斥锁：保护队列的并发访问
    pthread_cond_t cond_not_full;       // 条件变量：队列非满（生产者等待）
    pthread_cond_t cond_not_empty;      // 条件变量：队列非空（消费者等待）
} MsgQueue;

// ************************** 消息队列核心操作 **************************

/**
 * 初始化消息队列
 * @param queue 待初始化的消息队列指针
 * @return 0: 成功，-1: 失败
 */
int msg_queue_init(MsgQueue *queue)
{
    if (queue == NULL)
    {
        perror("msg_queue_init: queue is NULL");
        return -1;
    }

    // 初始化队列索引和计数
    queue->front = 0;
    queue->rear = 0;
    queue->count = 0;

    // 初始化互斥锁
    if (pthread_mutex_init(&queue->mutex, NULL) != 0)
    {
        perror("pthread_mutex_init failed");
        return -1;
    }

    // 初始化条件变量
    if (pthread_cond_init(&queue->cond_not_full, NULL) != 0)
    {
        perror("pthread_cond_init (not_full) failed");
        pthread_mutex_destroy(&queue->mutex); // 清理已初始化的互斥锁
        return -1;
    }
    if (pthread_cond_init(&queue->cond_not_empty, NULL) != 0)
    {
        perror("pthread_cond_init (not_empty) failed");
        pthread_cond_destroy(&queue->cond_not_full); // 清理已初始化的条件变量
        pthread_mutex_destroy(&queue->mutex);        // 清理已初始化的互斥锁
        return -1;
    }

    return 0;
}

/**
 * 销毁消息队列（释放资源）
 * @param queue 待销毁的消息队列指针
 */
void msg_queue_destroy(MsgQueue *queue)
{
    if (queue == NULL)
    {
        return;
    }

    // 销毁互斥锁和条件变量（必须确保无线程再使用队列）
    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->cond_not_full);
    pthread_cond_destroy(&queue->cond_not_empty);

    // 重置队列参数（可选，防止野指针误用）
    queue->front = 0;
    queue->rear = 0;
    queue->count = 0;
}

/**
 * 发送消息（入队，生产者调用）
 * @param queue 消息队列指针
 * @param msg 待发送的消息
 * @return 0: 成功，-1: 失败
 */
int msg_queue_send(MsgQueue *queue, const Message *msg)
{
    if (queue == NULL || msg == NULL)
    {
        perror("msg_queue_send: invalid parameter");
        return -1;
    }

    // 1. 加互斥锁，保护队列操作
    if (pthread_mutex_lock(&queue->mutex) != 0)
    {
        perror("pthread_mutex_lock failed");
        return -1;
    }

    // 2. 等待队列非满（如果队列已满，阻塞当前线程）
    while (queue->count >= MAX_QUEUE_CAPACITY)
    {
        printf("生产者：消息队列已满，等待消费者取走消息...\n");
        // 阻塞时会自动释放互斥锁，被唤醒后重新获取互斥锁
        if (pthread_cond_wait(&queue->cond_not_full, &queue->mutex) != 0)
        {
            perror("pthread_cond_wait (not_full) failed");
            pthread_mutex_unlock(&queue->mutex); // 解锁后退出
            return -1;
        }
    }

    // 3. 消息入队（环形缓冲区，避免数组越界）
    memcpy(&queue->buffer[queue->rear], msg, sizeof(Message));
    queue->rear = (queue->rear + 1) % MAX_QUEUE_CAPACITY;
    queue->count++;

    printf("生产者：发送消息成功 | 类型：%d | 内容：%s | 当前队列消息数：%d\n",
           msg->msg_type, msg->msg_content, queue->count);

    // 4. 发送信号，通知消费者队列非空（有新消息可取）
    pthread_cond_signal(&queue->cond_not_empty);

    // 5. 释放互斥锁
    if (pthread_mutex_unlock(&queue->mutex) != 0)
    {
        perror("pthread_mutex_unlock failed");
        return -1;
    }

    return 0;
}

/**
 * 接收消息（出队，消费者调用）
 * @param queue 消息队列指针
 * @param msg 用于存储接收的消息（输出参数）
 * @return 0: 成功，-1: 失败
 */
int msg_queue_recv(MsgQueue *queue, Message *msg)
{
    if (queue == NULL || msg == NULL)
    {
        perror("msg_queue_recv: invalid parameter");
        return -1;
    }

    // 1. 加互斥锁，保护队列操作
    if (pthread_mutex_lock(&queue->mutex) != 0)
    {
        perror("pthread_mutex_lock failed");
        return -1;
    }

    // 2. 等待队列非空（如果队列为空，阻塞当前线程）
    while (queue->count <= 0)
    {
        printf("消费者：消息队列为空，等待生产者发送消息...\n");
        // 阻塞时会自动释放互斥锁，被唤醒后重新获取互斥锁
        if (pthread_cond_wait(&queue->cond_not_empty, &queue->mutex) != 0)
        {
            perror("pthread_cond_wait (not_empty) failed");
            pthread_mutex_unlock(&queue->mutex); // 解锁后退出
            return -1;
        }
    }

    // 3. 消息出队（环形缓冲区）
    memcpy(msg, &queue->buffer[queue->front], sizeof(Message));
    queue->front = (queue->front + 1) % MAX_QUEUE_CAPACITY;
    queue->count--;

    printf("消费者：接收消息成功 | 类型：%d | 内容：%s | 当前队列消息数：%d\n",
           msg->msg_type, msg->msg_content, queue->count);

    // 4. 发送信号，通知生产者队列非满（有空间可存新消息）
    pthread_cond_signal(&queue->cond_not_full);

    // 5. 释放互斥锁
    if (pthread_mutex_unlock(&queue->mutex) != 0)
    {
        perror("pthread_mutex_unlock failed");
        return -1;
    }

    return 0;
}

// ************************** 线程函数定义 **************************

/**
 * 生产者线程函数：循环发送消息
 * @param arg 消息队列指针（void* 适配 pthread 线程函数参数要求）
 * @return NULL
 */
void *producer_thread(void *arg)
{
    MsgQueue *queue = (MsgQueue *)arg;
    Message msg;
    int msg_id = 0;

    while (msg_id < 15)
    {
        msg.msg_type = 1; // 固定消息类型（可自定义区分）
        snprintf(msg.msg_content, MAX_MSG_CONTENT, "这是第 %d 条测试消息", msg_id + 1);

        msg_queue_send(queue, &msg); // 发送消息

        usleep(500000); // 500 毫秒 模拟生产耗时

        msg_id++;
    }

    printf("生产者：所有消息发送完毕，退出线程...\n");
    pthread_exit(NULL); // 退出线程
}

/**
 * 消费者线程函数：循环接收消息
 * @param arg 消息队列指针
 * @return NULL
 */
void *consumer_thread(void *arg)
{
    MsgQueue *queue = (MsgQueue *)arg;
    Message msg;
    int recv_count = 0;

    while (recv_count < 15)
    { // 接收 15 条消息后退出（与生产者对应）
        // 接收消息
        msg_queue_recv(queue, &msg);

        // 模拟消费耗时（可选，让效果更直观）
        usleep(1000000); // 1 秒

        recv_count++;
    }

    printf("消费者：所有消息接收完毕，退出线程...\n");
    pthread_exit(NULL);
}

// ************************** 主函数（程序入口） **************************
int main()
{
    MsgQueue msg_queue;
    pthread_t producer_tid, consumer_tid;

    // 1. 初始化消息队列
    if (msg_queue_init(&msg_queue) != 0)
    {
        fprintf(stderr, "消息队列初始化失败，程序退出\n");
        return EXIT_FAILURE;
    }

    // 2. 创建生产者线程和消费者线程
    if (pthread_create(&producer_tid, NULL, producer_thread, &msg_queue) != 0)
    {
        perror("pthread_create (producer) failed");
        msg_queue_destroy(&msg_queue);
        return EXIT_FAILURE;
    }
    if (pthread_create(&consumer_tid, NULL, consumer_thread, &msg_queue) != 0)
    {
        perror("pthread_create (consumer) failed");
        pthread_cancel(producer_tid); // 取消已创建的生产者线程
        msg_queue_destroy(&msg_queue);
        return EXIT_FAILURE;
    }

    // 3. 等待两个线程执行完毕
    if (pthread_join(producer_tid, NULL) != 0)
    {
        perror("pthread_join (producer) failed");
    }
    if (pthread_join(consumer_tid, NULL) != 0)
    {
        perror("pthread_join (consumer) failed");
    }

    // 4. 销毁消息队列，释放资源
    msg_queue_destroy(&msg_queue);

    printf("主线程：程序正常退出\n");
    return EXIT_SUCCESS;
}