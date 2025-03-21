#include "IPCManager.hpp"
#include <sys/inotify.h>
#include <unistd.h>
#include <limits.h>
#include <thread>
#include "FileWatcher.hpp"
#include "IPCManager.hpp"
#include <iostream>
#include <fstream>
#include <unistd.h>
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

void FileWatcher::addWatch(const std::string& filename) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    int wd = inotify_add_watch(m_inotifyFd, filename.c_str(), IN_MODIFY);
    if (wd < 0) {
        throw std::runtime_error("Failed to add watch for file: " + filename);
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
                    std::cerr << "Error reading inotify events" << std::endl;
                }
                break;
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
            m_ipcManager.sendUpdate(it->second, content);
        } catch (const std::exception& e) {
            std::cerr << "Error handling file update: " << e.what() << std::endl;
        }
    }
}
