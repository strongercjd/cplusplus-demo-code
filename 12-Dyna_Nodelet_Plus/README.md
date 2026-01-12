# 12-Dyna_Nodelet_Plus

基于 11-Dyna_Nodelet 增强版  
支持新增一个动态库，这个动态库不是节点，它提供一个接口提供给其他的动态库调用

修改点：

1. 支持json加载插件
2. libmessage_pub.so通过单例实现，并且使用 PIMPL(Pointer to Implementation) 模式
3. libjobexecutor.so通过C语言接口实现

必须先编译 libmessage_pub.so 因为 libalert_nodelet.so 依赖它
必须先编译 libjobexecutor.so 因为 job_nodelet.so 依赖它

备注：

- 修改接口，可以先编译libalert_nodelet.so（对应的nodelet_base.hpp肯定需要改），只要对应目录有libmessage_pub.so就可以。然后再编译libmessage_pub.so

- 不修改接口，修改完libalert_nodelet.cpp，编译 libalert_nodelet.so，只要对应目录有libmessage_pub.so就可以
