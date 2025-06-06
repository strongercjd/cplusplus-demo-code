#pragma once
#include <nlohmann/json.hpp>

namespace IPCProtocol {
    // 通信消息类型
    enum class MessageType {
        SUBSCRIBE,//订阅
        UNSUBSCRIBE,//取消订阅
        FILE_UPDATE,//文件更新
        SUBSCRIBE_RESPONSE, // 订阅响应类型
        AUTH_REQUEST,//认证
        AUTH_RESPONSE,//认证响应
        ERROR
    };

    // 基础消息结构
    struct Message {
        MessageType type;
        nlohmann::json payload;
        
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Message, type, payload)
    };

    // 文件更新通知结构
    struct FileUpdate {
        std::string filename;
        nlohmann::json content;
        
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(FileUpdate, filename, content)
    };

    // 订阅响应结构
    struct SubscribeResponse {
        bool success;
        std::string reason;
        
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(SubscribeResponse, success, reason)
    };

    // 认证请求结构
    struct AuthRequest {
        std::string username;
        std::string password;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(AuthRequest, username, password)
    };
    // 认证响应结构
    struct AuthResponse {
        bool success;
        std::string reason;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(AuthResponse, success, reason)
    };
}
