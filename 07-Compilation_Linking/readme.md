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

解读

* -lmath 链接libmath.so库，默认lib和.so是可以省略的
* -L./   -L 是「指定库搜索路径」的参数，./ 表示当前目录。默认编译器只搜索系统库路径，加上后编译器会在当前目录找 -lmath 指定的 libmath 库
