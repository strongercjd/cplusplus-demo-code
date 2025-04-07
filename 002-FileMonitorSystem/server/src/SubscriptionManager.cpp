#include "IPCManager.hpp"
#include <algorithm>
#include <iostream>

void SubscriptionManager::addSubscription(const std::string& filename, int client_fd) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_subscriptions[filename].insert(client_fd);    
}
/**
 * @brief 移除订阅者 
 * 
 * @param filename 文件名
 * @param client_fd 订阅者fd
 */
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
/**
 * @brief 获取订阅者列表 
 * 
 * @param filename 文件名
 * @return std::set<int> 订阅者列表 
 */
std::set<int> SubscriptionManager::getSubscribers(const std::string& filename) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_subscriptions.find(filename);
    if (it != m_subscriptions.end()) {
        return it->second;
    }
    return {};
}
