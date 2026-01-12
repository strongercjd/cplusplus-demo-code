#include <iostream>
#include <dlfcn.h>
#include <unordered_map>
#include <cstring>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

#include "nodelet_base.hpp"

#include "include/nlohmann/json.hpp"

using json = nlohmann::json;

// 插件加载器类
class NodeletLoader
{
public:
    // 从JSON文件加载配置并加载所有插件
    bool loadFromConfigFile(const std::string &config_file)
    {
        // 1. 读取JSON文件
        std::ifstream file(config_file);
        if (!file.is_open())
        {
            std::cerr << "无法打开配置文件: " << config_file << std::endl;
            return false;
        }

        json config_json;
        try
        {
            file >> config_json;
        }
        catch (const json::parse_error &e)
        {
            std::cerr << "JSON解析错误: " << e.what() << std::endl;
            return false;
        }

        // 2. 检查是否有plugins数组
        if (!config_json.contains("plugins") || !config_json["plugins"].is_array())
        {
            std::cerr << "配置文件格式错误：缺少plugins数组" << std::endl;
            return false;
        }

        // 3. 遍历并加载每个插件
        bool all_success = true;
        for (const auto &plugin_config : config_json["plugins"])
        {
            if (!plugin_config.contains("name") || !plugin_config.contains("library"))
            {
                std::cerr << "插件配置缺少必要字段（name或library）" << std::endl;
                all_success = false;
                continue;
            }

            std::string plugin_name = plugin_config["name"];
            std::string library_path = plugin_config["library"];

            // 将配置对象转换为JSON字符串
            std::string config_str;
            if (plugin_config.contains("config"))
            {
                config_str = plugin_config["config"].dump(); // 将JSON对象转为字符串
            }

            // 加载插件
            NodeletBase *nodelet = loadNodelet(
                library_path,
                plugin_name,
                config_str.c_str(),
                config_str.size());

            if (nodelet != nullptr)
            {
                nodelet->init();
                std::cout << "===== 插件 " << plugin_name << " 加载成功 =====\n"
                          << std::endl;
                // 存储插件实例（用于后续管理）
                loaded_plugins_[plugin_name] = nodelet;
            }
            else
            {
                std::cerr << "插件 " << plugin_name << " 加载失败" << std::endl;
                all_success = false;
            }
        }

        return all_success;
    }

    // 加载插件库并创建实例（保持原有接口）
    NodeletBase *loadNodelet(const std::string &lib_path,
                             const std::string &plugin_name,
                             const char *config,
                             int configSize)
    {
        void *handle = dlopen(lib_path.c_str(), RTLD_LAZY);
        if (!handle)
        {
            std::cerr << "加载插件库失败：" << dlerror() << std::endl;
            return nullptr;
        }

        dlerror();

        std::string create_func_name = "create_instance_" + plugin_name;
        typedef NodeletBase *(*CreateFunc)(const char *, int);
        CreateFunc create_func = (CreateFunc)dlsym(handle, create_func_name.c_str());

        const char *err = dlerror();
        if (err != nullptr)
        {
            std::cerr << "获取创建函数失败：" << err << std::endl;
            dlclose(handle);
            return nullptr;
        }

        std::string delete_func_name = "delete_instance_" + plugin_name;
        typedef void (*DeleteFunc)(NodeletBase *);
        DeleteFunc delete_func = (DeleteFunc)dlsym(handle, delete_func_name.c_str());
        if (dlerror() != nullptr)
        {
            std::cerr << "获取销毁函数失败" << std::endl;
            dlclose(handle);
            return nullptr;
        }

        handles_[plugin_name] = handle;
        delete_funcs_[plugin_name] = delete_func;

        return create_func(config, configSize);
    }

    // 销毁插件实例并关闭动态库
    void unloadNodelet(const std::string &plugin_name, NodeletBase *node)
    {
        // 先删除插件实例（在动态库关闭之前）
        if (delete_funcs_.count(plugin_name) && node != nullptr)
        {
            delete_funcs_[plugin_name](node);
        }

        // 然后关闭动态库
        if (handles_.count(plugin_name))
        {
            dlclose(handles_[plugin_name]);
            handles_.erase(plugin_name);
        }

        // 清理函数指针和插件实例记录
        delete_funcs_.erase(plugin_name);
        loaded_plugins_.erase(plugin_name);
    }

    // 销毁所有已加载的插件
    void unloadAll()
    {
        // 创建副本避免迭代器失效
        auto plugins_copy = loaded_plugins_;

        for (const auto &pair : plugins_copy)
        {
            // 检查是否还存在（可能已经被其他调用删除）
            if (loaded_plugins_.count(pair.first))
            {
                unloadNodelet(pair.first, pair.second);
            }
        }
    }

    ~NodeletLoader()
    {
        // 只调用unloadAll()，它会处理所有清理工作
        unloadAll();

        // 防御性清理：确保所有句柄都被关闭（正常情况下应该已经为空）
        while (!handles_.empty())
        {
            auto it = handles_.begin();
            dlclose(it->second);
            handles_.erase(it);
        }
    }

private:
    std::unordered_map<std::string, void *> handles_;
    std::unordered_map<std::string, void (*)(NodeletBase *)> delete_funcs_;
    std::unordered_map<std::string, NodeletBase *> loaded_plugins_; // 存储已加载的插件实例
};

// 主函数：支持从JSON配置文件加载
int main(int argc, char *argv[])
{
    NodeletLoader loader;

    // 如果提供了配置文件参数，则从文件加载
    if (argc > 1)
    {
        std::string config_file = argv[1];
        std::cout << "从配置文件加载插件: " << config_file << std::endl;

        if (loader.loadFromConfigFile(config_file))
        {
            std::cout << "\n所有插件加载完成" << std::endl;
        }
        else
        {
            std::cerr << "从配置文件加载插件失败" << std::endl;
            return 1;
        }
    }
    else
    {
        std::cout << "请提供配置文件路径" << std::endl;
    }
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}