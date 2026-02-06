#include "math.h"
#include <stdio.h>

int main()
{
    // 调用打印版本接口
    print_version();

    // 调用加法接口
    int a = 10, b = 5;
    printf("Result of %d + %d = %d\n", a, b, math_add(a, b));
    // printf("Result of %d - %d = %d\n", a, b, math_sub(a, b));

    return 0;
}