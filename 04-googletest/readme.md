# 编译  
如果不存在build文件夹，就新建  
mkdir build
``` 
cd build 
camke ..
```
然后make编译 在build中执行make  
```
make
```
# 运行  
生成 math_utils_test_demo  gtest_demo 和 math_demo  
* math_demo 可执行文件  
* math_utils_test_demo和gtest_demo 单元测试文件  
> 执行单元测试，可以直接执行 ./gtest_demo  也可以执行ctest