#include "IPCManager.hpp"
#include "FileWatcher.hpp"
#include <csignal>
#include <iostream>
#include <memory>

std::unique_ptr<IPCManager> g_ipcManager;
std::unique_ptr<FileWatcher> g_fileWatcher;

void signalHandler(int signum) {
    if (g_fileWatcher) {
        g_fileWatcher->stop();
    }
    if (g_ipcManager) {
        g_ipcManager->stop();
    }
    std::cout << "\nServer shutdown gracefully" << std::endl;
    exit(signum);
}

int main() {
    const std::string socketPath = "/tmp/ipc_socket.sock";
    
    try {
        /*
        不使用全局变量的原因是：
        1. 初始化顺序问题:全局变量的初始化顺序是不确定的，这可能导致依赖关系的问题。
        IPCManager manager(socketPath);  // 这里构造会立即执行，但此时可能其他全局变量尚未初始化
        FileWatcher watcher(manager);   // 这里构造会依赖manager，但此时manager可能尚未初始化
        2. 异常安全：当前设计将对象的构造放在try块中，可以更好地捕获构造函数抛出的异常
        3. 全局变量的初始化和销毁是在程序启动和结束时进行的，而不是在函数调用时进行的。
        */
        g_ipcManager = std::make_unique<IPCManager>(socketPath);// 初始化IPC管理器
        g_fileWatcher = std::make_unique<FileWatcher>(*g_ipcManager);// 初始化文件监控
        
        try
        {
            g_fileWatcher->addWatch(PROJECT_ROOT"/server/configs/app.json");
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return EXIT_FAILURE;
        }
        
        
        // 注册信号处理
        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);
        
        // 启动服务
        g_fileWatcher->start();
        g_ipcManager->start();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
         // 清理资源
         g_fileWatcher.reset();
         g_ipcManager.reset();
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
