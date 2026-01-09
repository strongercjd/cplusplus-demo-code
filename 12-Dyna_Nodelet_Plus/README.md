和 12-Dyna_Nodelet_Plus 对比修改点：
1. 支持json加载插件
2. 支持新增一个动态库，这个动态库不是节点，它提供一个接口提供给其他的动态库调用

必须先编译 libmessage_pub.so 因为 libalert_nodelet.so 依赖它