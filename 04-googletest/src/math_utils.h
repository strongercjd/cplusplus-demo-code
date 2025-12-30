#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <stdexcept>

class MathUtils {
public:
    // 加法
    static int add(int a, int b);
    
    // 减法
    static int subtract(int a, int b);
    
    // 乘法
    static int multiply(int a, int b);
    
    // 除法（处理除零异常）
    static double divide(int a, int b);
    
    // 判断是否为偶数
    static bool isEven(int number);
};

#endif