#include "FileWatcher.hpp"
#include "IPCManager.hpp"
#include <sys/inotify.h>
#include <unistd.h>
#include <limits.h>
#include <thread>

#include <iostream>
#include <fstream>
#include <sys/inotify.h>
#include <stdexcept>

#define BUF_LEN (1024 * (sizeof(struct inotify_event) + 16))

#define EVENT_SIZE  (sizeof(struct inotify_event))
#define BUF_LEN     (1024 * (EVENT_SIZE + 16))

FileWatcher::FileWatcher(IPCManager& ipcManager) : 
    m_ipcManager(ipcManager),
    m_inotifyFd(inotify_init1(IN_NONBLOCK)),
    m_running(false) {
    if (m_inotifyFd < 0) {
        throw std::runtime_error("Failed to initialize inotify");
    }
}

FileWatcher::~FileWatcher() {
    stop();
}

bool FileWatcher::isWatching(const std::string& filename) 
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& [wd, path] : m_watchedFiles) {
        // 从完整路径中提取纯文件名
        size_t pos = path.find_last_of("/\\");
        std::string baseName = (pos != std::string::npos) ? 
                             path.substr(pos + 1) : 
                             path;
        if (baseName == filename) return true;
    }
    return false;
}
void FileWatcher::addWatch(const std::string& filename) {
    std::lock_guard<std::mutex> lock(m_mutex);

    // 添加路径验证和错误细节
    if (access(filename.c_str(), F_OK) == -1) {
        throw std::runtime_error("File not found: " + filename + " (" + strerror(errno) + ")");
    }
    
    int wd = inotify_add_watch(m_inotifyFd, filename.c_str(), IN_MODIFY);
    if (wd < 0) {
        throw std::runtime_error("Failed to add watch for file: " + filename + " (" + strerror(errno) + ")");
    }

    m_watchedFiles[wd] = filename;
}

void FileWatcher::start() {
    m_running = true;
    std::thread([this]() {
        char buffer[BUF_LEN];
        while(m_running) {
            ssize_t length = read(m_inotifyFd, buffer, BUF_LEN);
            if (length < 0) {
                if (m_running) {
                    // 过滤非阻塞模式下的正常返回
                    if (errno != EAGAIN && errno != EWOULDBLOCK) {
                        std::cerr << "Error reading inotify events: " 
                                  << strerror(errno) << std::endl;
                    }
                    // 添加短暂休眠避免CPU占用
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                continue;  // 保持循环而不是退出
            }else{
                std::cout << "Read inotify events (" << length << " bytes)"<< std::endl;
            }

            for (char* ptr = buffer; ptr < buffer + length; ) {
                struct inotify_event* event = reinterpret_cast<struct inotify_event*>(ptr);
                if (event->mask & IN_MODIFY) {
                    handleFileModify(event->wd);
                }
                ptr += EVENT_SIZE + event->len;
            }
        }
    }).detach();
}

void FileWatcher::stop() {
    m_running = false;
    
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& [wd, path] : m_watchedFiles) {
        inotify_rm_watch(m_inotifyFd, wd);
    }
    m_watchedFiles.clear();
    
    close(m_inotifyFd);
}

void FileWatcher::handleFileModify(int wd) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_watchedFiles.find(wd);
    if (it != m_watchedFiles.end()) {
        try {
            std::ifstream file(it->second);
            nlohmann::json content;
            file >> content;

            // 添加JSON内容打印
            std::cout << "Modified JSON content (" << it->second << "):\n"
                      << content.dump(4) << "\n" << std::endl;
            // 提取纯文件名
            size_t pos = it->second.find_last_of("/\\");
            std::string filename = (pos != std::string::npos) ? 
                                  it->second.substr(pos + 1) : 
                                  it->second;
            
            m_ipcManager.sendUpdate(filename, content);

        } catch (const std::exception& e) {
            std::cerr << "Error handling file update: " << e.what() << std::endl;
        }
    }
}
