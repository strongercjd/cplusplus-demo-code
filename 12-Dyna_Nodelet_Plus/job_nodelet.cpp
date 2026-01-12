#include "nodelet_base.hpp"
#include <thread>
#include <atomic>
#include <functional>
#include "jobexecutor_interface.hpp"

// 继承NodeletBase并实现接口
class JobNodelet : public NodeletBase
{
private:
    std::thread job_thread_;
    std::atomic<bool> running_;

    void job_task2()
    {
        std::cout << "[JobNodelet] 执行任务2...  " << std::endl;
    }
    void job_task3()
    {
        std::cout << "[JobNodelet] 执行任务3...  " << std::endl;
    }

    // 任务函数
    void job_task()
    {
        running_ = true;
        while (running_)
        {
            std::cout << "[JobNodelet] 执行任务...  " << std::endl;
            JobExecutorRun();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        std::cout << "[JobNodelet] 任务线程退出" << std::endl;
    }

public:
    // 构造函数：调用基类构造函数
    JobNodelet(const char *config, int configSize) : NodeletBase(config, configSize), running_(false) {}

    ~JobNodelet()
    {
        if (running_)
        {
            running_ = false;
            if (job_thread_.joinable())
            {
                job_thread_.join();
            }
        }
    }

    // 实现初始化接口
    bool init() override
    {
        std::cout << "[JobNodelet] 初始化插件..." << std::endl;
        // 模拟读取配置（比如告警阈值）
        if (getConfig() != nullptr)
        {
            std::cout << "[JobNodelet] 加载配置: " << std::string(getConfig(), getConfigSize()) << std::endl;
        }
        JobExecutorAddJob("job2", std::bind(&JobNodelet::job_task2, this), 1000, "job2");
        JobExecutorAddJob("job3", std::bind(&JobNodelet::job_task3, this), 1500, "job3");
        // 启动任务线程
        running_ = true;
        job_thread_ = std::thread(&JobNodelet::job_task, this);
        std::cout << "[JobNodelet] 任务线程已启动" << std::endl;
        return true; // 初始化成功
    }
};

// 导出插件：标识为"job"，生成create_instance_alert/delete_instance_alert函数
NODELET_EXPORT_CLASS(JobNodelet, job)