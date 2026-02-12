# readme

在linux下运行

## demo功能示例

* 01-CommandModelFactory 这是一个基于工厂模式和命令模式的设备模型管理系统，支持通过命令行动态创建不同设备模型（A/B/C/D）并处理特定指令（AA/WS/QS/ER）
* 02-FileMonitorSystem  基于进程间通信（IPC）的实时文件监控系统，服务器端监控文件变化并通过 Unix socket 实时推送更新给订阅的客户端 thread
* 03-AStarDemo A*算法的demo
* 04-googletest gtest的演示demo
* 05-SignalsAndSlots 使用C++ 简单实现一下类似QT的信号与槽机制
* 06-message_bus 消息订阅/发布机制和超时回调功能的消息总线系统，用于在客户端处理异步消息通信（如C/S架构中的请求-响应），当消息在指定时间内未收到响应时会自动触发超时回调
* 07-Compilation_Linking 静态编译和动态编译 (C语言)
* 08-Dynamic_Loading 动态加载 (C语言)
* 09-Plugin_Framework_C 使用C语言构建的动态库，插件加载框架 (C语言)
* 10-Plugin_Framework_Plus_C 根据 09-Plugin_Framework_C 修改的完善版插件框架 (C语言)
* 11-Dyna_Nodelet C++ 动态插件加载系统
* 12-Dyna_Nodelet_Plus 基于 11-Dyna_Nodelet 增强版
* 13-Timer 使用C++的thread实现定时器功能
* 14-thread_msg_queue_C 使用C语言实现自定义消息队列线程间通信 pthread
* 15-IPC_sysv 使用System V 消息队列的跨进程通信 C++ 演示项目
* 16-libversion 主要展示了动态库版本兼容的问题
* 17-backward_cpp C++程序崩溃时栈信息的抓取
* 18-backward_cpp_lite 17的精简版融入具体的项目

## IPC 进程通信

* 02-FileMonitorSystem  Unix socket 进程通信
* 15-IPC_sysv System V 消息队列

## 线程通信

* 14-thread_msg_queue_C 线程通信

## 多线程

* std::thread
  * 02-FileMonitorSystem
  * 13-Timer

* pthread
  * 14-thread_msg_queue_C
