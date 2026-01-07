# 编译与链接

## 静态编译

编译步骤

``` bash
$gcc -c main.cpp
$gcc -c math.cpp
$gcc main.o math.o -o main
$ ./main
Hello World!
result:10
```

## 动态链接

``` bash
$gcc -shared -fPIC math.cpp -o libmath.so
$gcc main.cpp -lmath -L./ -o main
$export LD_LIBRARY_PATH="$(pwd)"
$ ./main
Hello World!
result:10
```
