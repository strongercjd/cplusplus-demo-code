# 插件框架

使用C语言 linux系统编译

该架构便于模块解耦、动态加载和独立编译，适合插件化系统或可扩展应用

编译命令

```bash
gcc -fPIC -shared data_sender.c -o libdata_sender.so
gcc -fPIC -shared data_processor.c -ldata_sender -L./ -o libdata_processor.so
gcc main.c -ldata_processor -ldata_sender -L./ -o main
```

执行效果

```bash
$ ./main 
Hello World!
data_processor start...
data_sender start...
data_processor_timer count=1
data_processor_timer count=2
```
