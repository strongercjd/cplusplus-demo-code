#include "ClientHandler.hpp"

ClientHandler::~ClientHandler() = default;
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <thread>

ClientHandler::ClientHandler(const std::string& socketPath) : 
    m_socketPath(socketPath), 
    m_running(false) {

    // 创建客户端套接字
    m_socketFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (m_socketFd < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    // 连接服务器
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socketPath.c_str(), sizeof(addr.sun_path)-1);

    if (connect(m_socketFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(m_socketFd);
        throw std::runtime_error("Failed to connect to server");
    }
}

void ClientHandler::subscribe(const std::string& filename) {
    IPCProtocol::Message msg;
    msg.type = IPCProtocol::MessageType::SUBSCRIBE;
    msg.payload["filename"] = filename;
    
    std::string data = nlohmann::json(msg).dump();
    //打印data
    std::cout << "subscribe data: " << data << std::endl;
    write(m_socketFd, data.c_str(), data.size());
}

void ClientHandler::unsubscribe(const std::string& filename) {
    IPCProtocol::Message msg;
    msg.type = IPCProtocol::MessageType::UNSUBSCRIBE;
    msg.payload["filename"] = filename;
    
    std::string data = nlohmann::json(msg).dump();
    write(m_socketFd, data.c_str(), data.size());
}

void ClientHandler::startListening(UpdateCallback callback) {
    m_running = true;
    m_callback = callback;
    
    m_listenerThread = std::thread([this]() {
        char buffer[4096];
        while (m_running) {
            ssize_t count = read(m_socketFd, buffer, sizeof(buffer));
            if (count <= 0) {
                if (m_running) {
                    std::cerr << "Connection lost" << std::endl;
                }
                break;
            }
            //打印接收到的消息
            std::cout << "Client received message: " << buffer << std::endl;
            try {
                auto msg = nlohmann::json::parse(buffer, buffer + count);
                auto protocolMsg = msg.get<IPCProtocol::Message>();
                
                if (protocolMsg.type == IPCProtocol::MessageType::FILE_UPDATE) {
                    auto update = protocolMsg.payload.get<IPCProtocol::FileUpdate>();
                    if (m_callback) {
                        m_callback(update.filename, update.content);
                    }
                }
                // 消息处理部分
                if (protocolMsg.type == IPCProtocol::MessageType::SUBSCRIBE_RESPONSE) {
                    auto resp = protocolMsg.payload.get<IPCProtocol::SubscribeResponse>();
                    if (resp.success) {
                        std::cout << "Subscribe success " << std::endl;
                    } else {
                        std::cerr << "Subscribe failed: " << resp.reason << std::endl;
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Error processing message: " << e.what() << std::endl;
            }
        }
    });
}

void ClientHandler::stop() {
    m_running = false;
    if (m_listenerThread.joinable()) {
        m_listenerThread.join();
    }
    close(m_socketFd);
}
