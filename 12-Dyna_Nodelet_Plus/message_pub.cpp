#include "message_pub.hpp"
#include <iostream>

// 实现类：包含所有实现细节
class MessagePub::Impl
{
public:
    Impl() : count_(0), running_(true)
    {
        // 启动计数器线程
        counter_thread_ = std::thread(&MessagePub::Impl::counterThread, this);
    }

    ~Impl()
    {
        // 停止计数器线程
        running_ = false;
        if (counter_thread_.joinable())
        {
            counter_thread_.join();
        }
    }

    int getCount() const
    {
        return count_.load();
    }

private:
    // 计数器线程函数
    void counterThread()
    {
        while (running_)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            if (running_)
            {
                count_++;
            }
        }
    }

    // 计数器
    std::atomic<int> count_;

    // 线程控制
    std::thread counter_thread_;
    std::atomic<bool> running_;
};
// MessagePub 的实现
MessagePub::MessagePub() : pImpl_(new Impl())
{
}

MessagePub::~MessagePub()
{
    delete pImpl_;
}

MessagePub &MessagePub::getInstance()
{
    static MessagePub instance;
    return instance;
}

int MessagePub::getCount() const
{
    return pImpl_->getCount();
}