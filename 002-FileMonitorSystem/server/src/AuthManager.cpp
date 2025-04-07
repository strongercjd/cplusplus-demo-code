#include "AuthManager.hpp"

AuthManager::AuthManager()
{
    addCredential("admin", "admin");
    addCredential("test", "123456");
}

/**
 * @brief 添加用户凭据
 *
 * @param username 用户名
 * @param password 密码
 */
void AuthManager::addCredential(const std::string& username, const std::string& password)
{
    m_credentials[username] = password;
}

/**
 * @brief 验证用户凭据
 * 
 * @param username 用户名
 * @param password 密码
 * @return true 
 * @return false 
 */
bool AuthManager::validateCredentials(const std::string& username, const std::string& password) 
{
    auto it = m_credentials.find(username);
    return it != m_credentials.end() && it->second == password;
}



