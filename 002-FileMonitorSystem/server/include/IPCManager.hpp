#pragma once
#include <string>
#include <unordered_map>
#include <set>
#include <mutex>
#include "Protocol.hpp"

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

private:
    void handleClientConnection();
    void handleMessage(const IPCProtocol::Message& msg, int client_fd);

    std::string m_socketPath;
    int m_serverFd = -1;
    bool m_running;
    SubscriptionManager m_subManager;
};
