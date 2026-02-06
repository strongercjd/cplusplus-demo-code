#ifndef MATH_H
#define MATH_H

// 打印当前库的版本信息
void print_version();

// 整数加法函数（两个 int 类型参数，返回 int 结果）
int math_add(int a, int b);

// 整数减法函数（1.1.0 版本新增，不破坏原有接口）
int math_sub(int a, int b);

#endif