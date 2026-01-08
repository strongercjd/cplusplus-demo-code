# 插件框架

使用C语言 linux系统编译

## 优化说明

该架构便于模块解耦、动态加载和独立编译，适合插件化系统或可扩展应用，基于09-Plugin_Framework_C完善的

编译命令在make.sh中

主要完善的点

- 编译时增加`-Wl,--allow-shlib-undefined"` 
    - -Wl, 是 gcc 把后续参数传递给链接器 ld 的标识,
    - --allow-shlib-undefined 让链接器忽略「共享库中未定义符号」的错误，允许生成可执行文件，这些未定义的符号会在程序运行时，由动态加载的共享库（.so）提供
- 采用json维护需要加载的动态库

达到的效果：
- 可以单独修改某个库文件，重新启动main就可以。
- 新增加插件只需要修改json就行，不用重新编译main，main就是主框架，不需要修改

## 注意

main.c 打开动态库之前的代码是`handle = dlopen("./libdata_processor.so", RTLD_LAZY);`现在是`handle = dlopen(libname, RTLD_LAZY | RTLD_GLOBAL);`

为什么需要 **RTLD_GLOBAL**？

RTLD_GLOBAL 的作用：

* 使用 RTLD_GLOBAL：动态库的符号会被添加到全局符号表，后续加载的库可以访问这些符号
* 不使用 RTLD_GLOBAL（仅 RTLD_LAZY）：符号是局部的，其他库无法访问当 libdata_processor.so 加载时，它需要解析 get_count()。如果 libdata_sender.so 是用 RTLD_LAZY（无 RTLD_GLOBAL）加载的，其符号不会进入全局符号表，libdata_processor.so 就找不到 get_count()。
