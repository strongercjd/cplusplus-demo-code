# 目标：生成可执行文件 sysv_msg_queue
sysv_msg_queue: sysv_msg_queue.c
	gcc sysv_msg_queue.c -o sysv_msg_queue -lpthread

# 伪目标：清理生成的可执行文件
.PHONY: clean
clean:
	rm -f sysv_msg_queue