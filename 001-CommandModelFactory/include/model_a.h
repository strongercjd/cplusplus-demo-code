#pragma once
#include <unordered_map>
#include <functional>
#include <string>

class ModelA {
protected:
    std::unordered_map<std::string, std::function<void(const std::string&)>> command_handlers;
    void registerCommand(const std::string& cmd, std::function<void(const std::string&)> handler);
    
    // 基础命令处理函数
    void actioncmdAA(const std::string& content);
    void actioncmdWS(const std::string& content);
    void actioncmdQS(const std::string& content);
    void actioncmdER(const std::string& content);

public:
    ModelA();
    virtual ~ModelA() = default;
    virtual void registerCommands();
    void handleCommand(const std::string& cmd, const std::string& content);
};