#include "math.h"
#include <stdio.h>

// 打印版本：1.1.0
void print_version()
{
    printf("=== math Library Version: 1.1.0 === \n");
}

// 实现整数加法
int math_add(int a, int b)
{
    return a + b;
}

// 新增：实现整数减法
int math_sub(int a, int b)
{
    return a - b;
}