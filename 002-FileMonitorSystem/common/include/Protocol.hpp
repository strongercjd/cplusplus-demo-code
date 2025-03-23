#pragma once
#include <nlohmann/json.hpp>

namespace IPCProtocol {
    // 通信消息类型
    enum class MessageType {
        SUBSCRIBE,//订阅
        UNSUBSCRIBE,//取消订阅
        FILE_UPDATE,//文件更新
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
}
