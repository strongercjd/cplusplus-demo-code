# readme

# 编译

链接libmath.so

```bash
gcc main.c -o main -L. -lmath -Wl,-rpath=./
```

* -L.：在当前目录查找动态库
* -lmydemo：链接 libmydemo.so（开发用软链接）
* -Wl,-rpath=./：让可执行文件记住运行时在当前目录查找动态库（避免后续设置环境变量）
