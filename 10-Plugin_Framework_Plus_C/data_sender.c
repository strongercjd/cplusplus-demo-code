#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int count = 0;

// 获取count
int get_count()
{
	return count;
}

// 核心定时器函数：每隔100ms打印数据
void *data_sender_timer(void *arg)
{
	// 仅用于标识打印次数
	// 控制定时器运行（单线程场景，普通int即可）
	int running = 1;

	while (running)
	{
		// 每隔100ms执行一次打印
		usleep(100000);
		count++;
	}

	return NULL;
}

int data_sender_start()
{
	printf("data_sender start...\n");

	// 启动定时器线程
	pthread_t timer_thread;
	pthread_create(&timer_thread, NULL, data_sender_timer, NULL);
	pthread_detach(timer_thread);

	return 0;
}