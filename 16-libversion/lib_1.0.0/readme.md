# readme

# 编译

编译生成实际动态库文件：libmath.so.1.1.0

-Wl,-soname=libmath.so.1：设置该库的 SONAME 为 libmath.so.1

```bash
gcc -fPIC -shared -o libmath.so.1.0.0 math.c -Wl,-soname=libmath.so.1
```

## 软连接

按照 Linux 动态库的规范，创建两个软链接：

```bash
# SONAME 软链接（名称 = SONAME，指向实际库文件）
ln -s libmath.so.1.0.0 libmath.so.1

# 开发用软链接（无版本号，指向 SONAME 软链接，方便编译时使用）
ln -s libmath.so.1 libmath.so
```

## 验证 1.0.0 版本动态库的 SONAME

用 readelf 命令验证 SONAME 是否设置成功：

```bash
$ readelf -d libmath.so.1.0.0 | grep SONAME
 0x000000000000000e (SONAME)             Library soname: [libmath.so.1]
```
