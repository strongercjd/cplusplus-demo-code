#include "nodelet_base.hpp"
#include <thread>
#include <atomic>
#include "message_pub.hpp"

// 告警插件类：继承NodeletBase并实现接口
class AlertNodelet : public NodeletBase
{
private:
    std::thread alert_thread_;
    std::atomic<bool> running_;

    // 告警任务函数
    void alert_task()
    {
        running_ = true;
        while (running_)
        {
            std::cout << "[AlertNodelet] 执行告警任务... 消息总数: "<< MessagePub::getInstance().getCount() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        } 
        std::cout << "[AlertNodelet] 告警任务线程退出" << std::endl;
    }

public:
    // 构造函数：调用基类构造函数
    AlertNodelet(const char *config, int configSize) : NodeletBase(config, configSize), running_(false){}

    ~AlertNodelet()
    {
        if (running_)
        {
            running_ = false;
            if (alert_thread_.joinable())
            {
                alert_thread_.join();
            }
        }
    }
    // 实现初始化接口
    bool init() override
    {
        std::cout << "[AlertNodelet] 初始化插件..." << std::endl;
        // 模拟读取配置（比如告警阈值）
        if (getConfig() != nullptr)
        {
            std::cout << "[AlertNodelet] 加载配置: " << std::string(getConfig(), getConfigSize()) << std::endl;
        }
        // 启动告警任务线程
        running_ = true;
        alert_thread_ = std::thread(&AlertNodelet::alert_task, this);
        std::cout << "[AlertNodelet] 告警任务线程已启动" << std::endl;
        return true; // 初始化成功
    }
};

// 导出插件：标识为"alert"，生成create_instance_alert/delete_instance_alert函数
NODELET_EXPORT_CLASS(AlertNodelet, alert)