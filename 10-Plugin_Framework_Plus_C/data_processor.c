#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

extern int get_count();

// 核心定时器函数：每隔100ms打印数据
void *data_processor_timer(void *arg)
{
	int count = 0; // 仅用于标识打印次数
	// 控制定时器运行（单线程场景，普通int即可）
	int running = 1;

	while (running)
	{
		// 每隔150ms执行一次打印
		usleep(150000);

		count = get_count();

		printf("data_processor_timer count=%d\r\n", count);
	}

	return NULL;
}

int data_processor_start()
{
	printf("data_processor start...\n");

	// 启动定时器线程
	pthread_t timer_thread;
	pthread_create(&timer_thread, NULL, data_processor_timer, NULL);
	pthread_detach(timer_thread);

	return 0;
}