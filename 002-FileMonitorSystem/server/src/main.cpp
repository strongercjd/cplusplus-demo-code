#include "IPCManager.hpp"
#include "FileWatcher.hpp"
#include <csignal>
#include <iostream>

IPCManager* g_ipcManager = nullptr;
FileWatcher* g_fileWatcher = nullptr;

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
        // 初始化IPC管理器
        IPCManager manager(socketPath);
        g_ipcManager = &manager;
        
        // 初始化文件监控
        FileWatcher watcher(manager);
        g_fileWatcher = &watcher;
        try
        {
            watcher.addWatch(PROJECT_ROOT"/server/configs/app.json");
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
        watcher.start();
        manager.start();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
