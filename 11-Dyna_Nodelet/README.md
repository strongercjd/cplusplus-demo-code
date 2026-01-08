# DynaNodelet - C++ 动态插件加载系统

这是一个基于 C++ 的动态插件加载系统演示项目，模拟了 ROS 的 Nodelet 机制。该项目展示了如何在运行时动态加载和卸载 C++ 插件，实现灵活的插件化架构。

## 项目概述

本项目实现了一个轻量级的插件框架，允许在运行时动态加载 C++ 插件库，而无需重新编译主程序。系统通过 `dlopen`/`dlsym` 实现动态库的加载和符号解析。

## 项目结构

```bash
.
├── nodelet_base.hpp      # 插件基类定义和导出宏
├── alert_nodelet.cpp     # 示例插件：告警插件
├── plugin_loader.cpp     # 插件加载器实现
├── Makefile              # 构建脚本
└── README.md             # 项目说明文档
```

## 核心组件

### 1. NodeletBase（插件基类）

`nodelet_base.hpp` 定义了所有插件必须继承的基类：

- **构造函数**：接收配置参数（模拟 ROS 的参数配置）
- **纯虚函数**：
  - `init()`: 插件初始化，必须由子类实现
  - `run()`: 插件核心运行逻辑，必须由子类实现
- **辅助函数**：`getConfig()` 和 `getConfigSize()` 用于获取配置数据

### 2. NODELET_EXPORT_CLASS 宏

用于导出插件类，生成标准的创建和销毁函数：

```cpp
NODELET_EXPORT_CLASS(AlertNodelet, alert)
```

该宏会生成：
- `create_instance_alert()`: 创建插件实例
- `delete_instance_alert()`: 销毁插件实例

### 3. NodeletLoader（插件加载器）

`plugin_loader.cpp` 实现了插件的动态加载和管理：

- **loadNodelet()**: 加载插件库并创建实例
- **unloadNodelet()**: 销毁插件实例并关闭动态库
- 自动管理动态库句柄和销毁函数

### 4. AlertNodelet（示例插件）

`alert_nodelet.cpp` 是一个示例插件实现，展示了如何：

- 继承 `NodeletBase` 基类
- 实现 `init()` 和 `run()` 方法
- 使用 `NODELET_EXPORT_CLASS` 宏导出插件

## 编译和运行

### 前置要求

- Linux 系统（使用 `dlopen`/`dlsym`）
- GCC 编译器（支持 C++11）
- Make 工具

### 编译步骤

```bash
# 编译所有目标（插件库和加载器）
make

# 或者分别编译
make libalert_nodelet.so  # 编译插件动态库
make plugin_loader        # 编译插件加载器
```

### 运行

```bash
# 运行插件加载器
./plugin_loader
```

### 清理

```bash
# 清理编译产物
make clean
```

## 工作原理

1. **编译阶段**：
   - 插件代码编译为动态库（`.so` 文件）
   - 使用 `-fPIC` 选项生成位置无关代码
   - 通过 `NODELET_EXPORT_CLASS` 宏导出创建/销毁函数

2. **运行时加载**：
   - 使用 `dlopen()` 打开动态库
   - 使用 `dlsym()` 获取 `create_instance_*` 函数
   - 调用创建函数实例化插件对象

3. **插件使用**：
   - 调用 `init()` 初始化插件
   - 调用 `run()` 执行插件逻辑

4. **卸载**：
   - 调用销毁函数释放插件对象
   - 使用 `dlclose()` 关闭动态库

## 创建新插件

要创建新的插件，只需：

1. 创建一个新的 `.cpp` 文件（如 `my_plugin.cpp`）
2. 继承 `NodeletBase` 并实现 `init()` 和 `run()` 方法
3. 使用 `NODELET_EXPORT_CLASS` 宏导出插件
4. 在 `Makefile` 中添加编译规则
5. 编译为动态库（`.so` 文件）

示例：

```cpp
#include "nodelet_base.hpp"

class MyPlugin : public NodeletBase {
public:
    MyPlugin(const char *config, int configSize) 
        : NodeletBase(config, configSize) {}
    
    bool init() override {
        // 初始化逻辑
        return true;
    }
    
    void run() override {
        // 运行逻辑
    }
};

NODELET_EXPORT_CLASS(MyPlugin, myplugin)
```

## 注意事项

- 本项目仅支持 Linux 系统（使用 `dlopen`/`dlsym`）
- Windows 系统需要使用 `LoadLibrary`/`GetProcAddress` 替代
- 确保插件库路径正确，否则加载会失败
- 插件标识（如 "alert"）必须与导出宏中的标识一致
