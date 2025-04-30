#include "IPCManager.hpp"
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdexcept>
#include <thread>
#include <iostream>
#include <vector>
#include "FileWatcher.hpp"
#include "AuthManager.hpp"

using namespace std;

IPCManager::~IPCManager() = default;

IPCManager::IPCManager(const string& socketPath) : 
    m_socketPath(socketPath), 
    m_fileWatcher(nullptr),
    m_running(false) {

    // 创建UNIX域套接字
    m_serverFd = socket(AF_UNIX, SOCK_STREAM, 0);// // 设置地址族为Unix域，也就是本地通信，SOCK_STREAM表示流式 0表示使用默认协议
    if (m_serverFd < 0) {// 检查套接字创建是否失败
        throw runtime_error("Failed to create socket");
    }

    // 绑定socket地址
    //sockaddr_un是UNIX域套接字的地址结构,un是UNIX的缩写
    //sockaddr_in是网络通信套接字的地址结构，in是internet的缩写
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;// 设置地址族为Unix域
    strncpy(addr.sun_path, socketPath.c_str(), sizeof(addr.sun_path)-1);//// 复制文件路径（防止溢出）

    unlink(socketPath.c_str()); // 移除已有socket文件

    if (bind(m_serverFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {//绑定套接字到指定路径,如果绑定失败，关闭套接字并抛出异常
        close(m_serverFd);
        throw runtime_error("Failed to bind socket");
    }

    if (listen(m_serverFd, 5) < 0) {// 监听套接字,如果监听失败，关闭套接字并抛出异常
        close(m_serverFd);
        throw runtime_error("Failed to listen on socket");
    }
}
/**
 * @brief 启动IPCManager,开始监听客户端连接 
 * 
 */
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
        t.join();// 等待所有工作线程结束
    }
}

void IPCManager::stop() {
    m_running = false;
    close(m_serverFd);
    unlink(m_socketPath.c_str());
}
/**
 * @brief 处理客户端连接 
 * 
 */
void IPCManager::handleClientConnection() {
    struct sockaddr_un client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    int client_fd = accept(m_serverFd, (struct sockaddr*)&client_addr, &client_len);// 接受客户端连接
    if (client_fd < 0) {
        if (m_running) {
            std::cerr << "Accept failed" << std::endl;
        }
        return;
    }

    //打印客户端id
    std::cout << "Client connected client_fd: " << client_fd << std::endl;

    // 处理客户端消息
    thread([this, client_fd]() {
        char buffer[4096];
        while(true) {
            ssize_t count = read(client_fd, buffer, sizeof(buffer));
            if (count <= 0) {
                break;
            }
            //打印接收到的消息
            std::cout << "Received message: " << buffer << std::endl;
            // 反序列化消息
            try {
                auto msg = nlohmann::json::parse(buffer, buffer + count);
                handleMessage(msg.get<IPCProtocol::Message>(), client_fd);
            } catch (const exception& e) {
                std::cerr << "Message parse error: " << e.what() << std::endl;
            }
            // 清空buffer
            memset(buffer, 0, sizeof(buffer));
        }
        close(client_fd);
    }).detach();// 分离线程
    /*
    分离线程讲解：
    1.分离线程的好处
      * 使新线程独立运行，不再与主线程保持关联
      * 主线程不用等待这个新线程结束（非阻塞）
      * 线程资源会在执行完成后自动回收
    2.与join的区别
      * join会阻塞主线程，直到新线程执行完成
      * join会等待新线程结束后才会继续执行主线程
    3.当前代码场景
      * 用于异步处理客户端连接
      * 每个客户端连接都会创建独立的后台线程
      * 主线程可以继续接受新的客户端连接
      * 线程内部已妥善处理了client_fd的关payload闭操作
    */
}
/**
 * @brief 处理客户端消息
 * 
 * @param msg 消息析后的消息对象
 * @param client_fd 客户端fd 
 */
void IPCManager::handleMessage(const IPCProtocol::Message& msg, int client_fd) {
    switch(msg.type) {
        case IPCProtocol::MessageType::SUBSCRIBE: {
            string filename = msg.payload["filename"];

            if (!m_fileWatcher) {
                throw std::runtime_error("FileWatcher not bound");
                return;
            }
            
            // 文件校验逻辑
            if (!m_fileWatcher->isWatching(filename)) {
                // 发送失败响应
                IPCProtocol::SubscribeResponse resp{false, "File not monitored"};
                IPCProtocol::Message responseMsg{
                    IPCProtocol::MessageType::SUBSCRIBE_RESPONSE,
                    resp
                };
                string data = nlohmann::json(responseMsg).dump();
                write(client_fd, data.c_str(), data.size());
                return;
            }
            
            m_subManager.addSubscription(filename, client_fd);
            
            // 发送成功响应
            IPCProtocol::SubscribeResponse resp{true, ""};
            IPCProtocol::Message responseMsg{
                IPCProtocol::MessageType::SUBSCRIBE_RESPONSE,
                resp
            };
            string data = nlohmann::json(responseMsg).dump();
            write(client_fd, data.c_str(), data.size());
            break;
        }
        case IPCProtocol::MessageType::UNSUBSCRIBE: {
            string filename = msg.payload["filename"];
            m_subManager.removeSubscription(filename, client_fd);
            break;
        }
        case IPCProtocol::MessageType::AUTH_REQUEST: {
            string username = msg.payload["username"];
            string password = msg.payload["password"];
            IPCProtocol::AuthResponse resp;
            // 验证凭据
            if (m_authManager.validateCredentials(username, password)) {
                // 发送成功响应
                resp = {true, ""};
                //存储客户端验证成功的client_fd
                setClientAuthStatus(client_fd, true);
            }else {
                // 发送失败响应
                resp = {false, "Authentication failed"};
                setClientAuthStatus(client_fd, false);
            }
            IPCProtocol::Message responseMsg{
                IPCProtocol::MessageType::AUTH_RESPONSE,
                resp
            };
            string data = nlohmann::json(responseMsg).dump();
            write(client_fd, data.c_str(), data.size());
            break;
        }
        default:
            std::cerr << "Unhandled message type" << std::endl;
    }
}
/**
 * @brief  发送文件更新消息
 * 
 * @param filename  文件名
 * @param content   文件内容
 */
void IPCManager::sendUpdate(const string& filename, const nlohmann::json& content) {
    IPCProtocol::FileUpdate update{filename, content};
    IPCProtocol::Message msg{IPCProtocol::MessageType::FILE_UPDATE, update};

    auto clients = m_subManager.getSubscribers(filename);
    //打印clients大小
    std::cout << "Subscribers for [" << filename << "] size: " << clients.size() << std::endl;
    //打印clients每个元素
    for (int fd : clients) {
        std::cout << "Subscribers for [" << filename << "]: fd=" << fd << std::endl;
    }
    for (int fd : clients) {
        string data = nlohmann::json(msg).dump();// 序列化消息
        //static_cast<void> 用于显式忽略返回值（避免未使用返回值的编译警告）
        static_cast<void>(write(fd, data.c_str(), data.size()));// 发送消息给订阅者
    }
}
/**
 * @brief  绑定文件监视器
 *
 * @param fileWatcher  文件监视器
 */
void IPCManager::bindFileWatcher(FileWatcher& fileWatcher) 
{
    m_fileWatcher = &fileWatcher;
}
/**
 * @brief 设置客户端验证状态
 * 
 * @param clientId 客户端id
 * @param isAuthenticated 状态
 */
void IPCManager::setClientAuthStatus(int clientId, bool isAuthenticated) {
    m_authenticatedClients[clientId] = isAuthenticated;
}
/**
 * @brief 获取客户端验证状态
 * 
 * @param clientId 客户端id
 * @return true 
 * @return false 
 */
bool IPCManager::isClientAuthenticated(int clientId) {
    return m_authenticatedClients.find(clientId) != m_authenticatedClients.end();
}
