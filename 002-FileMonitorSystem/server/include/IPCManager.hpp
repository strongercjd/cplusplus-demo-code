#pragma once
#include <string>
#include <unordered_map>
#include <set>
#include <mutex>
#include "Protocol.hpp"
#include "AuthManager.hpp" 

class FileWatcher;
class AuthManager;

// 订阅管理器
class SubscriptionManager {
public:
    void addSubscription(const std::string& filename, int client_fd);
    void removeSubscription(const std::string& filename, int client_fd);
    std::set<int> getSubscribers(const std::string& filename);

private:
    std::mutex m_mutex;
    std::unordered_map<std::string, std::set<int>> m_subscriptions;
};

class IPCManager {
public:
    explicit IPCManager(const std::string& socketPath);
    ~IPCManager();

    void start();
    void stop();
    void sendUpdate(const std::string& filename, const nlohmann::json& content);

    void bindFileWatcher(FileWatcher& fileWatcher);

    void setClientAuthStatus(int clientId, bool isAuthenticated);//设置客户端验证状态
    bool isClientAuthenticated(int clientId);//获取客户端验证状态

private:
    void handleClientConnection();
    void handleMessage(const IPCProtocol::Message& msg, int client_fd);

    AuthManager m_authManager; // 认证管理器

    std::unordered_map<int, bool> m_authenticatedClients;//客户端验证状态
    

    FileWatcher* m_fileWatcher;

    std::string m_socketPath;
    int m_serverFd = -1;
    bool m_running;
    SubscriptionManager m_subManager;
};
