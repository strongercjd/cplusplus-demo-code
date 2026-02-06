# readme

主要演示实现一个完整的自定义动态库 demo，既要验证「小版本更新（SONAME 不变）的兼容性」，也要验证「跨版本更新（SONAME 改变）的不兼容性」，全程不依赖第三方库（仅用系统标准库）

## 编译并运行1.0.0版本的库

```bash
cd lib_1.0.0/
gcc -fPIC -shared -o libmath.so.1.0.0 math.c -Wl,-soname=libmath.so.1
gcc -fPIC -shared -o libmath.so math.c -Wl,-soname=libmath.so.1
```

编译出 libmath.so.1.0.0 和 libmath.so  

将 libmath.so 拷贝到./app/中用，用于上层应员开发

app编译时需要链接libmath.so

```bash
gcc main.c -o main -L. -lmath -Wl,-rpath=./
```

将 libmath.so 拷贝到./exec/中用，还有app中的main也要拷贝到./exec/中

在exec中创建软连接

``` bash
ln -s libmath.so.1.0.0 libmath.so.1
```

运行程序

```bash
$ LD_LIBRARY_PATH=./ ./main
=== math Library Version: 1.0.0 === 
Result of 10 + 5 = 15
```

这样 app 编译时，他的目录中的 libmath.so 不用动，1.0.0 版本的库就可以自行编译，编译出来的库文件，直接放到exec目录中，重新运行就会自动加载

## 编译并运行1.1.0版本的库

比如我有个需求，需要释放1.1.0版本的库，当然可以在lib_1.0.0目录中直接修改，但是为了演示更清晰，新建一个 lib_1.1.0 目录。把lib_1.0.0文件拷贝到这里

对代码进行修改，增加math_sub函数，并编译

```bash
gcc -fPIC -shared -o libmath.so.1.1.0 math.c -Wl,-soname=libmath.so.1
```

这个时候，只需要把 libmath.so.1.1.0 拷贝到exec目录中，并删除之前的软连接，新建软连接

```bash
rm libmath.so.1
ln -s libmath.so.1.1.0 libmath.so.1
```

> 这个时候，exec中的 libmath.so.1.0.0 其实已经没用了，可以删除，可以保留，无所谓

这个时候再运行

```bash
$ LD_LIBRARY_PATH=./ ./main
=== math Library Version: 1.1.0 === 
Result of 10 + 5 = 15
```

开发迭代中

1. app没有用到1.1.0版本新增的API，这个是app目录中的头文件math.h是不需要更换的，当然更换时最好的。
2. app使用到了新的api，需要再lib_1.1.0中新编译出libmath.so，并放置到app目录中，并且更新头文件math.h。编译出新的main。**注意** 如果这个是，exec中的库文件没有更新，还是1.0.0版本的，就会报错符号未找到

    ```bash
    $ LD_LIBRARY_PATH=./ ./main
    ./main: symbol lookup error: ./main: undefined symbol: math_sub
    ```

    编译main时如下，就可以在执行到没有符号的位置才报错，而不会运行过程中报错

    ```bash
    $ gcc main.c -o main -L. -lmath -Wl,-z,lazy
    $ ./main
    === math Library Version: 1.0.0 ===
    Result of 10 + 5 = 15
    ./main: symbol lookup error: ./main: undefined symbol: math_sub
    ```

    编译参数说明
    各参数说明：  
    - -L.：指定动态库的搜索路径为当前目录（.），否则 ld 会只去系统默认路径（/lib、/usr/lib 等）找库；  
    - -lmath：链接名为libmath.so的动态库（ld 会自动补全lib和.so）；  
    - -Wl,-z,lazy：开启延迟符号绑定，核心选项。  

    > 这个 -Wl,-z,lazy 对可执行文件有效，对动态库生效的办法见《10-Plugin_Framework_Plus_C/readme.md》

## 编译并错误运行2.1.0版本

将lib_2.0.0编译出的libmath.so.2.0并创建软连接

```bash
$ ln -s libmath.so.2.0.0 libmath.so.2
$ ./main 
./main: error while loading shared libraries: libmath.so.1: cannot open shared object file: No such file or directory
```

运行报错，找不到 libmath.so.1。这是因为编译APP的main时，它依赖的libmath.so的SONAME就是libmath.so.1

强制修改libmath.so.2.0.0为libmath.so.1，欺骗动态链接器

```bash
$ rm libmath.so.2
$ ln -s libmath.so.2.0.0 libmath.so.1
 ./main 
=== math new Library Version: 2.0.0 === 
Segmentation fault (core dumped)
```

直接触发了错误，所以欺骗动态链接器的方式，可以正常运行没有被修改的接口。

简单来说：虽然库版本不兼容，但是你使用的是之前兼容的接口，就不影响。只有运行到不兼容的接口时才会报错。强制修改，会产生一种软件可以正常运行的错觉。
