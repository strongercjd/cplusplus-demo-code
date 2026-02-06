# readme

# 编译

编译生成实际动态库文件：libmath.so.2.0.0

-Wl,-soname=libmath.so.2：设置该库的 SONAME 为 libmath.so.2

```bash
gcc -fPIC -shared -o libmath.so.2.0.0 math.c -Wl,-soname=libmath.so.2
```

## 验证 1.1.0 版本动态库的 SONAME

用 readelf 命令验证 SONAME 是否设置成功：

```bash
$ readelf -d libmath.so.2.0.0 | grep SONAME
 0x000000000000000e (SONAME)             Library soname: [libmath.so.1]
```
