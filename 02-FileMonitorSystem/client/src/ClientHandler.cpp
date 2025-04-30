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
/**
 * @brief 启动客户端 发送认证请求
 * 
 * @param filename 文件名 
 */
void ClientHandler::start(const std::string& filename) {
    m_filename = filename;
    std::string inputUser, inputPass;
    std::cout << "请输入账户: ";
    std::cin >> inputUser;
    
    std::cout << "请输入密码: ";
    std::cin >> inputPass;
    IPCProtocol::Message authMsg;
    authMsg.type = IPCProtocol::MessageType::AUTH_REQUEST;
    authMsg.payload["username"] = inputUser;
    authMsg.payload["password"] = inputPass;

    std::string authData = nlohmann::json(authMsg).dump();
    write(m_socketFd, authData.c_str(), authData.size());
}
/**
 * @brief 订阅文件 
 * 
 * @param filename 文件名 
 */
void ClientHandler::subscribe(const std::string& filename) {
    IPCProtocol::Message msg;
    msg.type = IPCProtocol::MessageType::SUBSCRIBE;
    msg.payload["filename"] = filename;
    
    std::string data = nlohmann::json(msg).dump();
    //打印data
    std::cout << "subscribe data: " << data << std::endl;
    write(m_socketFd, data.c_str(), data.size());
}

/**
 * @brief 取消订阅
 * 
 * @param callback filename 文件名
 */
void ClientHandler::unsubscribe(const std::string& filename) {
    IPCProtocol::Message msg;
    msg.type = IPCProtocol::MessageType::UNSUBSCRIBE;
    msg.payload["filename"] = filename;
    
    std::string data = nlohmann::json(msg).dump();
    write(m_socketFd, data.c_str(), data.size());
}
/**
 * @brief 开始监听
 * 
 * @param callback 文件更新回调函数
 */
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

                switch (protocolMsg.type)
                {
                case IPCProtocol::MessageType::FILE_UPDATE:{
                    auto update = protocolMsg.payload.get<IPCProtocol::FileUpdate>();
                    if (m_callback) {
                        m_callback(update.filename, update.content);
                    }
                    break;
                }
                case IPCProtocol::MessageType::SUBSCRIBE_RESPONSE:{
                    auto resp = protocolMsg.payload.get<IPCProtocol::SubscribeResponse>();
                    if (resp.success) {
                        std::cout << "Subscribe success " << std::endl;
                    } else {
                        std::cerr << "Subscribe failed: " << resp.reason << std::endl;
                    }
                    break;
                }
                case IPCProtocol::MessageType::AUTH_RESPONSE:{
                    auto resp = protocolMsg.payload.get<IPCProtocol::AuthResponse>();
                    if (resp.success) {
                        std::cout << "🎉 Auth success " << std::endl;
                        // 认证成功，开始订阅文件
                        if (!m_filename.empty()) {
                            subscribe(m_filename);
                        }
                    }else{
                        // 认证失败，退出程序
                        std::cerr << "❌ Auth failed: " << resp.reason << std::endl;
                        m_running = false;
                        break;
                    }
                }
                default:
                    break;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error processing message: " << e.what() << std::endl;
            }
        }
    });
}
/**
 * @brief 停止监听
 * 
 */
void ClientHandler::stop() {
    m_running = false;
    shutdown(m_socketFd, SHUT_RDWR); // 强制中断阻塞的read调用
    if (m_listenerThread.joinable()) {
        m_listenerThread.join();
    }
    close(m_socketFd);
}
/**
 * @brief 检查是否正在运行
 * 
 * @return true 
 * @return false 
 */
bool ClientHandler::isRunning() const {
    return m_running;
}
