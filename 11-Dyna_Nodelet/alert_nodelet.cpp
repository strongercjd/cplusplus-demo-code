#include "nodelet_base.hpp"

// 模拟一个告警插件：继承NodeletBase并实现接口
class AlertNodelet : public NodeletBase
{
public:
    // 构造函数：调用基类构造函数
    AlertNodelet(const char *config, int configSize) : NodeletBase(config, configSize) {}

    // 实现初始化接口
    bool init() override
    {
        std::cout << "[AlertNodelet] 初始化插件..." << std::endl;
        // 模拟读取配置（比如告警阈值）
        if (getConfig() != nullptr)
        {
            std::cout << "[AlertNodelet] 加载配置: " << std::string(getConfig(), getConfigSize()) << std::endl;
        }
        return true; // 初始化成功
    }

    // 实现运行接口
    void run() override
    {
        std::cout << "[AlertNodelet] 运行告警逻辑..." << std::endl;
        std::cout << "[AlertNodelet] 检查设备状态：正常" << std::endl;
    }
};

// 导出插件：标识为"alert"，生成create_instance_alert/delete_instance_alert函数
NODELET_EXPORT_CLASS(AlertNodelet, alert)