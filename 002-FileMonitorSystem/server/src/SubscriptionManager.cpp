#include "IPCManager.hpp"
#include <algorithm>
#include <iostream>

void SubscriptionManager::addSubscription(const std::string& filename, int client_fd) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_subscriptions[filename].insert(client_fd);

    // 打印m_subscriptions所有内容
    // std::cout << "m_subscriptions: " << std::endl;
    // for (const auto& [filename, value] : m_subscriptions) {
    //     // 打印filename和value
    //     for (int fd : value) {
    //         std::cout << "Key: " << filename << ", client id: "<<fd<<std::endl;
    //     }
    // }
    
}

void SubscriptionManager::removeSubscription(const std::string& filename, int client_fd) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_subscriptions.find(filename);
    if (it != m_subscriptions.end()) {
        it->second.erase(client_fd);
        if (it->second.empty()) {
            m_subscriptions.erase(it);
        }
    }
}

std::set<int> SubscriptionManager::getSubscribers(const std::string& filename) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_subscriptions.find(filename);
    if (it != m_subscriptions.end()) {
        return it->second;
    }
    return {};
}
