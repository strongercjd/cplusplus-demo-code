#ifndef NODELET_BASE_H
#define NODELET_BASE_H

#include <iostream>
#include <cstring>

// 模拟Nodelet基类：所有插件必须继承该类并实现纯虚函数
class NodeletBase
{
public:
    // 构造函数：接收配置参数（模拟ROS的参数配置）
    NodeletBase(const char *config, int configSize)
    {
        if (config && configSize > 0)
        {
            config_data_ = new char[configSize];
            memcpy(config_data_, config, configSize);
            config_size_ = configSize;
        }
        else
        {
            config_data_ = nullptr;
            config_size_ = 0;
        }
    }

    // 析构函数：释放内存
    virtual ~NodeletBase()
    {
        if (config_data_)
        {
            delete[] config_data_;
        }
    }

    // 纯虚函数：插件初始化（必须由子类实现）
    virtual bool init() = 0;

    // 纯虚函数：插件核心运行逻辑（必须由子类实现）
    virtual void run() = 0;

    // 获取配置数据（辅助函数）
    const char *getConfig() const { return config_data_; }
    int getConfigSize() const { return config_size_; }

private:
    char *config_data_; // 存储配置数据
    int config_size_;   // 配置数据长度
};

// 模拟插件导出宏：对应你之前看到的PLUGINLIB_EXPORT_CLASS
// 参数说明：n = 插件类名，m = 插件标识（用于生成唯一函数名）
#define NODELET_EXPORT_CLASS(n, m)                                                         \
    extern "C"                                                                             \
    {                                                                                      \
        /* 设置函数可见性，确保动态库能导出该函数 */                                       \
        __attribute__((visibility("default")))                                             \
        NodeletBase *create_instance_##m(const char *config, const int configSize)         \
        {                                                                                  \
            return (NodeletBase *)(new n(config, configSize));                             \
        }                                                                                  \
        __attribute__((visibility("default"))) void delete_instance_##m(NodeletBase *node) \
        {                                                                                  \
            delete node;                                                                   \
        }                                                                                  \
    }

#endif // NODELET_BASE_H