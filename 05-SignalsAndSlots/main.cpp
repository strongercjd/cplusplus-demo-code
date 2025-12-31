#include "signal.hpp"
#include <iostream>
#include <string>

// 普通函数作为槽
void slot1(const std::string &message)
{
    std::cout << "槽1 收到消息: " << message << std::endl;
}
// 普通函数作为槽
void slot2(const std::string &message)
{
    std::cout << "槽2 收到消息: " << message << std::endl;
}
// 测试类，拥有自己的槽
class TestClass
{
public:
    // 成员函数作为槽
    void classSlot(const std::string &message)
    {
        std::cout << "TestClass::classSlot 收到消息: " << message << std::endl;
    }
};

int main()
{
    // 创建一个信号
    Signal<std::string> signal;
    // 连接普通函数到信号
    auto id1 = signal.connect(slot1);
    auto id2 = signal.connect(slot2);
    // 创建一个类实例，并连接成员函数到信号
    TestClass obj;
    auto id3 = signal.connect([&obj](const std::string &message)
                              { obj.classSlot(message); });
    // 第一次触发信号，所有槽都会被调用
    std::cout << "第一次触发信号：" << std::endl;
    signal.emit("你好，信号与槽！");
    // 从信号中断开槽1
    std::cout << "\n断开槽1后,第二次触发信号:" << std::endl;
    signal.disconnect(id1);
    // 第二次触发信号，仅槽2和成员函数槽会被调用
    signal.emit("这是第二条消息！");
    return 0;
}