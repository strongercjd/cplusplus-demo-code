#include "math_utils.h"
/**
 * @brief 加法
 * 
 * @param a 加数
 * @param b 加数
 * @return int 和
 */
int MathUtils::add(int a, int b)
{
    return a + b;
}

/**
 * @brief 减法
 * 
 * @param a 被减数
 * @param b 减数
 * @return int 差
 */
int MathUtils::subtract(int a, int b)
{
    return a - b;
}

/**
 * @brief 乘法
 * 
 * @param a 乘数
 * @param b 乘数
 * @return int 积
 */
int MathUtils::multiply(int a, int b)
{
    return a * b;
}

/**
 * @brief 除法
 * 
 * @param a 被除数
 * @param b 除数
 * @return double 商
 */
double MathUtils::divide(int a, int b)
{
    if (b == 0)
    {
        throw std::invalid_argument("Division by zero is not allowed");
    }
    return static_cast<double>(a) / b;
}

/**
 * @brief 判断是否为偶数
 * 
 * @param number 数字
 * @return true 是偶数
 * @return false 不是偶数
 */
bool MathUtils::isEven(int number)
{
    return number % 2 == 0;
}
