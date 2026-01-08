#include <iostream>
#include <dlfcn.h>       // Linux动态库加载头文件（核心）
#include <unordered_map> // 添加这一行
#include <cstring>       // 添加这一行（用于 strlen）
#include "nodelet_base.hpp"

// 插件加载器类
class NodeletLoader
{
public:
    // 加载插件库并创建实例
    // 参数：lib_path = 插件库路径，plugin_name = 插件标识（如"alert"），config = 配置数据
    NodeletBase *loadNodelet(const std::string &lib_path,
                             const std::string &plugin_name,
                             const char *config,
                             int configSize)
    {
        // 1. 打开动态库（RTLD_LAZY：延迟解析符号）
        void *handle = dlopen(lib_path.c_str(), RTLD_LAZY);
        if (!handle)
        {
            std::cerr << "加载插件库失败：" << dlerror() << std::endl;
            return nullptr;
        }

        // 2. 重置错误
        dlerror();

        // 3. 获取创建实例的函数（拼接函数名：create_instance_ + plugin_name）
        std::string create_func_name = "create_instance_" + plugin_name;
        typedef NodeletBase *(*CreateFunc)(const char *, int);
        CreateFunc create_func = (CreateFunc)dlsym(handle, create_func_name.c_str());

        // 检查函数是否获取成功
        const char *err = dlerror();
        if (err != nullptr)
        {
            std::cerr << "获取创建函数失败：" << err << std::endl;
            dlclose(handle);
            return nullptr;
        }

        // 4. 获取销毁实例的函数（备用，存储句柄和销毁函数）
        std::string delete_func_name = "delete_instance_" + plugin_name;
        typedef void (*DeleteFunc)(NodeletBase *);
        DeleteFunc delete_func = (DeleteFunc)dlsym(handle, delete_func_name.c_str());
        if (dlerror() != nullptr)
        {
            std::cerr << "获取销毁函数失败" << std::endl;
            dlclose(handle);
            return nullptr;
        }

        // 5. 存储句柄和销毁函数（用于后续释放）
        handles_[plugin_name] = handle;
        delete_funcs_[plugin_name] = delete_func;

        // 6. 创建插件实例
        return create_func(config, configSize);
    }

    // 销毁插件实例并关闭动态库
    void unloadNodelet(const std::string &plugin_name, NodeletBase *node)
    {
        // 1. 调用销毁函数
        if (delete_funcs_.count(plugin_name))
        {
            delete_funcs_[plugin_name](node);
        }

        // 2. 关闭动态库
        if (handles_.count(plugin_name))
        {
            dlclose(handles_[plugin_name]);
            handles_.erase(plugin_name);
            delete_funcs_.erase(plugin_name);
        }
    }

    ~NodeletLoader()
    {
        // 析构时关闭所有未释放的库
        for (auto &pair : handles_)
        {
            dlclose(pair.second);
        }
    }

private:
    // 存储动态库句柄（key：插件标识）
    std::unordered_map<std::string, void *> handles_;
    // 存储销毁函数（key：插件标识）
    std::unordered_map<std::string, void (*)(NodeletBase *)> delete_funcs_;
};

// 主函数：测试插件加载和运行
int main()
{
    // 1. 创建加载器
    NodeletLoader loader;

    // 2. 配置数据（模拟传给插件的参数）
    const char *config = "alert_threshold=0.8;log_level=INFO";
    int configSize = strlen(config);

    // 3. 加载插件（注意：插件库路径要和编译后的路径一致）
    NodeletBase *alert_nodelet = loader.loadNodelet(
        "./libalert_nodelet.so", // 插件库文件
        "alert",                 // 插件标识
        config,                  // 配置数据
        configSize               // 配置长度
    );

    // 4. 使用插件
    if (alert_nodelet != nullptr)
    {
        std::cout << "\n===== 插件加载成功 =====" << std::endl;
        alert_nodelet->init(); // 初始化插件
        alert_nodelet->run();  // 运行插件逻辑
        std::cout << "===== 插件运行完成 =====\n"
                  << std::endl;

        // 5. 销毁插件
        loader.unloadNodelet("alert", alert_nodelet);
        std::cout << "插件已销毁" << std::endl;
    }

    return 0;
}