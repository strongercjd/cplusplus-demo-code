#include "timer.hpp"
#include <iostream>

// 自定义演示类（包含成员方法作为定时回调）
class DemoClass
{
private:
    int execute_count_ = 0;  // 成员变量：记录方法执行次数
    std::mutex count_mutex_; // 互斥锁：保证成员变量线程安全

public:
    // 成员方法：作为定时器的回调函数
    void timer_member_callback()
    {
        // 加锁保护成员变量，避免多线程竞争
        std::lock_guard<std::mutex> lock(count_mutex_);
        execute_count_++;

        std::cout << "[类成员方法任务] 执行次数: " << execute_count_
                  << " | 时间戳(ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() % 10000 << std::endl;
    }

    // 获取执行次数（供外部查看）
    int get_execute_count()
    {
        std::lock_guard<std::mutex> lock(count_mutex_);
        return execute_count_;
    }
};

// 永久循环任务（500ms一次）
void always_loop_task()
{
    static int count = 0;
    std::cout << "[永久循环任务] 执行次数: " << ++count
              << " | 时间戳(ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() % 10000 << std::endl;
}

// 用于演示暂停/恢复的循环任务（1000ms一次）
void pause_demo_task()
{
    static int count = 0;
    std::cout << "[暂停演示任务] 执行次数: " << ++count
              << " | 时间戳(ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() % 10000 << std::endl;
}

// 单次执行任务（2000ms后执行）
void once_task()
{
    std::cout << "[单次执行任务] 执行完成 | 时间戳(ms): "
              << std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::system_clock::now().time_since_epoch())
                         .count() %
                     10000
              << std::endl;
}

int main()
{
    Timer timer;
    std::cout << "===== 定时器启动，注册所有任务 =====" << std::endl;

    // 1. 注册【永久循环任务】：500ms执行一次，全程不暂停/取消
    uint64_t always_loop_id = timer.add_task(always_loop_task, 500, true);
    std::cout << "注册永久循环任务，ID: " << always_loop_id << std::endl;

    // 2. 注册【暂停演示任务】：1000ms执行一次，用于演示暂停/恢复/取消
    uint64_t pause_demo_id = timer.add_task(pause_demo_task, 1000, true);
    std::cout << "注册暂停演示任务，ID: " << pause_demo_id << std::endl;

    // 3. 注册【普通单次任务】：2000ms后执行一次
    uint64_t once_task_id = timer.add_task(once_task, 2000, false);
    std::cout << "注册普通单次任务，ID: " << once_task_id << std::endl;

    // 4. 注册【Lambda单次任务】：3000ms后执行一次
    uint64_t lambda_once_id = timer.add_task([]()
                                             { std::cout << "[Lambda单次任务] 执行完成 | 时间戳(ms): "
                                                         << std::chrono::duration_cast<std::chrono::milliseconds>(
                                                                std::chrono::system_clock::now().time_since_epoch())
                                                                    .count() %
                                                                10000
                                                         << std::endl; }, 3000, false);
    std::cout << "注册Lambda单次任务，ID: " << lambda_once_id << std::endl;

    // 5. 新增：注册【类成员方法任务】：800ms执行一次（循环）
    DemoClass demo_obj; // 创建自定义类实例
    // 方式1：用std::bind绑定成员方法（推荐，清晰直观）
    // uint64_t class_member_id = timer.add_task(std::bind(&DemoClass::timer_member_callback, &demo_obj), 800, true);
    // 方式2：用lambda捕获实例指针（更灵活，支持传参）
    uint64_t class_member_id = timer.add_task([&demo_obj]()
                                              { demo_obj.timer_member_callback(); }, 800, true);
    std::cout << "注册类成员方法任务，ID: " << class_member_id << std::endl;

    // 阶段1：运行4秒，观察所有任务正常执行（包括类成员方法任务）
    std::cout << "\n===== 阶段1：运行4秒，所有任务正常执行 =====" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(4));

    // 阶段2：暂停【暂停演示任务】，运行3秒，观察该任务停止执行
    std::cout << "\n===== 阶段2：暂停演示任务，运行3秒 =====" << std::endl;
    if (timer.pause_task(pause_demo_id))
    {
        std::cout << "成功暂停演示任务（ID: " << pause_demo_id << "）" << std::endl;
    }
    else
    {
        std::cout << "暂停演示任务失败（ID: " << pause_demo_id << "）" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // 阶段3：恢复【暂停演示任务】，运行3秒，观察该任务恢复执行
    std::cout << "\n===== 阶段3：恢复演示任务，运行3秒 =====" << std::endl;
    if (timer.resume_task(pause_demo_id))
    {
        std::cout << "成功恢复演示任务（ID: " << pause_demo_id << "）" << std::endl;
    }
    else
    {
        std::cout << "恢复演示任务失败（ID: " << pause_demo_id << "）" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // 阶段4：取消【暂停演示任务】，运行2秒，观察该任务彻底停止
    std::cout << "\n===== 阶段4：取消演示任务，运行2秒 =====" << std::endl;
    if (timer.cancel_task(pause_demo_id))
    {
        std::cout << "成功取消演示任务（ID: " << pause_demo_id << "）" << std::endl;
    }
    else
    {
        std::cout << "取消演示任务失败（ID: " << pause_demo_id << "）" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // 新增：打印类成员方法任务的总执行次数
    std::cout << "\n===== 类成员方法任务总执行次数：" << demo_obj.get_execute_count() << " =====" << std::endl;

    // 阶段5：程序收尾，停止定时器
    std::cout << "\n===== 阶段5：程序结束，停止定时器 =====" << std::endl;
    timer.stop();
    std::cout << "定时器已停止，程序退出" << std::endl;

    return 0;
}