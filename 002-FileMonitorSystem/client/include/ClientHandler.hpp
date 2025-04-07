#pragma once
#include <string>
#include <functional>
#include <thread>
#include "Protocol.hpp"

class ClientHandler {
public:
    using UpdateCallback = std::function<void(const std::string&, const nlohmann::json&)>;

    explicit ClientHandler(const std::string& socketPath);
    ~ClientHandler();

    void subscribe(const std::string& filename);
    void unsubscribe(const std::string& filename);
    void startListening(UpdateCallback callback);
    void stop();
    void start(const std::string& filename);
    bool isRunning()const;

private:
    std::string m_socketPath;
    int m_socketFd = -1;
    bool m_running;
    std::thread m_listenerThread;
    UpdateCallback m_callback;
    std::string m_filename;
};
