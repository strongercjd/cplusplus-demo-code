#include "message_pub.hpp"
#include <iostream>

MessagePub::MessagePub() : count_(0), running_(true) {
    // 启动计数器线程
    counter_thread_ = std::thread(&MessagePub::counterThread, this);
}

MessagePub::~MessagePub() {
    // 停止计数器线程
    running_ = false;
    if (counter_thread_.joinable()) {
        counter_thread_.join();
    }
}

MessagePub& MessagePub::getInstance() {
    static MessagePub instance;
    return instance;
}

int MessagePub::getCount() const {
    return count_.load();
}

void MessagePub::counterThread() {
    while (running_) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (running_) {
            count_++;
        }
    }
}

