# 动态加载

``` bash
$ tree
.
├── main.c
└── math.c

1 directory, 2 files
$ gcc -shared -fPIC math.c -o libmath.so
$ gcc -ldl main.c -o main
$ ./main
Hello World!
Please enter a command (enter 'q' and press Enter to execute the add function, enter 'exit' to quit the program):
> q
Loading dynamic library...
Dynamic library loaded successfully.
start add(5,5)...
end add(5,5)=10...
> exit
Exiting program...
Dynamic library unloaded.
$.
```
