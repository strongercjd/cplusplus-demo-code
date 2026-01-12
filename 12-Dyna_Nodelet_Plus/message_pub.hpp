#ifndef MESSAGE_PUB_HPP
#define MESSAGE_PUB_HPP

#include <atomic>
#include <thread>
#include <chrono>
#include <mutex>

class MessagePub
{
public:
    // 获取单例实例
    static MessagePub &getInstance();

    // 获取计数值
    int getCount() const;

    // 禁止拷贝和赋值
    MessagePub(const MessagePub &) = delete;
    MessagePub &operator=(const MessagePub &) = delete;

    // 析构函数
    ~MessagePub();

private:
    // 私有构造函数
    MessagePub();

    // 计数器线程函数
    void counterThread();

    // PIMPL: 前向声明的实现类指针
    class Impl;
    Impl *pImpl_;
};

#endif // MESSAGE_PUB_HPP
