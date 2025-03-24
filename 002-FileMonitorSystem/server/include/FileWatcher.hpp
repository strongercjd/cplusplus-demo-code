#pragma once
#include <string>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <sys/inotify.h>

// 前向声明
class IPCManager;

class FileWatcher {
public:
    explicit FileWatcher(IPCManager& manager);
    ~FileWatcher();

    void addWatch(const std::string& filename);
    void start();
    void stop();
    bool isWatching(const std::string& filename);

private:
    void handleFileModify(int wd);

    IPCManager& m_ipcManager;          // 引用IPC管理器
    std::unordered_map<int, std::string> m_watchedFiles; // 监控的文件描述符与文件路径映射
    std::mutex m_mutex;                // 线程安全锁
    int m_inotifyFd;                   // inotify文件描述符
    std::atomic<bool> m_running{false};// 运行状态标志
};
