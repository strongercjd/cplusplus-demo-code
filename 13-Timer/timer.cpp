#include "timer.hpp"

Timer::Timer() : is_running_(true)
{
    worker_thread_ = std::thread(&Timer::worker, this);
}

Timer::~Timer()
{
    stop();
    if (worker_thread_.joinable()) // 如果线程可以join，则join
    {
        worker_thread_.join(); // join线程 等待线程结束
    }
}

uint64_t Timer::add_task(Callback callback, uint64_t interval_ms, bool is_repeat)
{
    std::lock_guard<std::mutex> lock(tasks_mutex_); // 加锁保护任务
    uint64_t task_id = next_task_id_++;             // 生成任务ID

    tasks_[task_id] = Task{
        task_id,
        std::move(callback),
        interval_ms,
        is_repeat,
        interval_ms,
        false,
        false};

    return task_id;
}

bool Timer::cancel_task(uint64_t task_id)
{
    std::lock_guard<std::mutex> lock(tasks_mutex_);
    auto it = tasks_.find(task_id);
    if (it != tasks_.end())
    {
        it->second.is_cancelled = true;
        tasks_.erase(it);
        return true;
    }
    return false;
}

bool Timer::pause_task(uint64_t task_id)
{
    std::lock_guard<std::mutex> lock(tasks_mutex_);
    auto it = tasks_.find(task_id);
    if (it != tasks_.end() && !it->second.is_cancelled && !it->second.is_paused)
    {
        it->second.is_paused = true;
        return true;
    }
    return false;
}

bool Timer::resume_task(uint64_t task_id)
{
    std::lock_guard<std::mutex> lock(tasks_mutex_);
    auto it = tasks_.find(task_id);
    if (it != tasks_.end() && !it->second.is_cancelled && it->second.is_paused)
    {
        it->second.is_paused = false;
        return true;
    }
    return false;
}

void Timer::stop()
{
    is_running_ = false;
}

void Timer::worker()
{
    const uint64_t sleep_step_ms = 10;
    while (is_running_)
    {
        auto start_time = std::chrono::steady_clock::now();
        std::vector<std::pair<uint64_t, Callback>> callbacks_to_execute;
        {
            std::lock_guard<std::mutex> lock(tasks_mutex_);
            for (auto it = tasks_.begin(); it != tasks_.end();)
            {
                Task &task = it->second;

                if (task.is_cancelled)
                {
                    it = tasks_.erase(it);
                    continue;
                }

                if (task.is_paused)
                {
                    ++it;
                    continue;
                }

                task.remaining_ms -= sleep_step_ms;

                if (task.remaining_ms <= 0)
                {
                    callbacks_to_execute.push_back(std::make_pair(task.id, task.callback));
                    if (task.is_repeat)
                    {
                        task.remaining_ms = task.interval_ms;
                        ++it;
                    }
                    else
                    {
                        it = tasks_.erase(it);
                    }
                }
                else
                {
                    ++it;
                }
            }
        } // 锁在这里释放

        // 在锁外执行所有回调函数
        for (auto &callback_pair : callbacks_to_execute)
        {
            uint64_t &task_id = callback_pair.first;
            Callback &callback = callback_pair.second;
            try
            {
                callback();
            }
            catch (const std::exception &e)
            {
                std::cerr << "Task " << task_id << " callback error: " << e.what() << std::endl;
            }
            catch (...)
            {
                std::cerr << "Task " << task_id << " callback unknown error" << std::endl;
            }
        }

        auto end_time = std::chrono::steady_clock::now();
        auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        uint64_t sleep_ms = (elapsed_ms < sleep_step_ms) ? (sleep_step_ms - elapsed_ms) : 0;

        if (sleep_ms > 0 && is_running_)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
        }
    }
}