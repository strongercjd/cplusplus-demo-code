#ifndef SIGNAL_H
#define SIGNAL_H

#include <unordered_map> // 用于存储槽的哈希表
#include <functional>    // 用于存储任意形式的槽函数
#include <iostream>      // 用于输出调试信息

// 信号类
template <typename... Args>
class Signal
{
public:
    using SlotType = std::function<void(Args...)>; // 定义槽的类型
    using SlotID = int;                            // 槽的唯一标识符
    // 连接一个槽，返回槽的唯一 ID
    SlotID connect(SlotType slot)
    {
        SlotID id = nextID++;
        slots[id] = slot; // 将槽存入哈希表
        return id;
    }
    // 断开一个槽，通过其唯一 ID
    void disconnect(SlotID id)
    {
        auto it = slots.find(id);
        if (it != slots.end())
        {
            slots.erase(it); // 从哈希表中移除槽
        }
    }
    // 触发信号，调用所有已连接的槽
    void emit(Args... args) const
    {
        for (const auto &pair : slots)
        {
            pair.second(args...); // 调用槽函数
        }
    }

private:
    std::unordered_map<SlotID, SlotType> slots; // 存储槽的哈希表
    SlotID nextID = 0;                          // 用于生成唯一 ID 的计数器
};
#endif