#pragma once

#include <unordered_map>
#include <string>

class AuthManager {
public:
    AuthManager();
    ~AuthManager(){}
    bool validateCredentials(const std::string& username, const std::string& password);// 验证用户凭据
    void addCredential(const std::string& username, const std::string& password);// 添加新用户
    
private:
    std::unordered_map<std::string, std::string> m_credentials; // 用户名->哈希密码
};









