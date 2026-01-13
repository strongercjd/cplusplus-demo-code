#include <iostream>
#include <unordered_map>
#include <functional>
#include <thread>
#include <mutex>
#include <chrono>
#include <atomic>
#include <memory>
#include <vector>

// 定时器类
class Timer
{
public:
    using Callback = std::function<void()>;
    Timer();
    ~Timer();
    uint64_t add_task(Callback callback, uint64_t interval_ms, bool is_repeat = false);
    bool cancel_task(uint64_t task_id);
    bool pause_task(uint64_t task_id);
    bool resume_task(uint64_t task_id);
    void stop();

private:
    struct Task
    {
        uint64_t id;           // 任务ID
        Callback callback;     // 回调函数
        uint64_t interval_ms;  // 间隔时间
        bool is_repeat;        // 是否重复
        uint64_t remaining_ms; // 剩余时间
        bool is_cancelled;     // 是否取消
        bool is_paused;        // 是否暂停
    };

    std::unordered_map<uint64_t, Task> tasks_;
    std::mutex tasks_mutex_;                // 任务互斥锁
    std::atomic<uint64_t> next_task_id_{1}; // 下一个任务ID
    std::atomic<bool> is_running_;          // 是否运行
    std::thread worker_thread_;             // 工作线程
    void worker();                          // 工作线程函数
};