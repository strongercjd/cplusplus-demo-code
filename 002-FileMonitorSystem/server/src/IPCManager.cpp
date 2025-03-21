#include "IPCManager.hpp"
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdexcept>
#include <thread>
#include <iostream>
#include <vector>

using namespace std;

IPCManager::~IPCManager() = default;

IPCManager::IPCManager(const string& socketPath) : 
    m_socketPath(socketPath), 
    m_running(false) {

    // 创建UNIX域套接字
    m_serverFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (m_serverFd < 0) {
        throw runtime_error("Failed to create socket");
    }

    // 绑定socket地址
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socketPath.c_str(), sizeof(addr.sun_path)-1);

    unlink(socketPath.c_str()); // 移除已有socket文件

    if (bind(m_serverFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(m_serverFd);
        throw runtime_error("Failed to bind socket");
    }

    // 开始监听
    if (listen(m_serverFd, 5) < 0) {
        close(m_serverFd);
        throw runtime_error("Failed to listen on socket");
    }
}

void IPCManager::start() {
    m_running = true;
    vector<thread> workers;

    // 启动10个工作线程处理客户端连接
    for(int i = 0; i < 10; ++i) {
        workers.emplace_back([this](){
            while(m_running) {
                handleClientConnection();
            }
        });
    }

    for(auto& t : workers) {
        t.join();
    }
}

void IPCManager::stop() {
    m_running = false;
    close(m_serverFd);
    unlink(m_socketPath.c_str());
}

void IPCManager::handleClientConnection() {
    struct sockaddr_un client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    int client_fd = accept(m_serverFd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        if (m_running) {
            std::cerr << "Accept failed" << std::endl;
        }
        return;
    }

    // 处理客户端消息
    thread([this, client_fd]() {
        char buffer[4096];
        while(true) {
            ssize_t count = read(client_fd, buffer, sizeof(buffer));
            if (count <= 0) {
                break;
            }

            // 反序列化消息
            try {
                auto msg = nlohmann::json::parse(buffer, buffer + count);
                handleMessage(msg.get<IPCProtocol::Message>(), client_fd);
            } catch (const exception& e) {
                std::cerr << "Message parse error: " << e.what() << std::endl;
            }
        }
        close(client_fd);
    }).detach();
}

void IPCManager::handleMessage(const IPCProtocol::Message& msg, int client_fd) {
    switch(msg.type) {
        case IPCProtocol::MessageType::SUBSCRIBE: {
            string filename = msg.payload["filename"];
            m_subManager.addSubscription(filename, client_fd);
            break;
        }
        case IPCProtocol::MessageType::UNSUBSCRIBE: {
            string filename = msg.payload["filename"];
            m_subManager.removeSubscription(filename, client_fd);
            break;
        }
        default:
            std::cerr << "Unhandled message type" << std::endl;
    }
}

void IPCManager::sendUpdate(const string& filename, const nlohmann::json& content) {
    IPCProtocol::FileUpdate update{filename, content};
    IPCProtocol::Message msg{IPCProtocol::MessageType::FILE_UPDATE, update};

    auto clients = m_subManager.getSubscribers(filename);
    for (int fd : clients) {
        string data = nlohmann::json(msg).dump();
        static_cast<void>(write(fd, data.c_str(), data.size()));
    }
}
