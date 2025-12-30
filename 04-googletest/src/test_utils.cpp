#include "test_utils.h"
#include <algorithm>
#include <cstdlib>

std::string StringProcessor::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string StringProcessor::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string StringProcessor::reverse(const std::string& str) {
    std::string result = str;
    std::reverse(result.begin(), result.end());
    return result;
}

bool StringProcessor::isEmpty(const std::string& str) {
    return str.empty();
}

size_t StringProcessor::length(const std::string& str) {
    return str.length();
}

void ExceptionDemo::throwRuntimeError() {
    throw std::runtime_error("Runtime error occurred");
}

void ExceptionDemo::throwInvalidArgument() {
    throw std::invalid_argument("Invalid argument");
}

void ExceptionDemo::throwLogicError() {
    throw std::logic_error("Logic error");
}

void ExceptionDemo::noThrow() {
    // 不抛出异常
}

void ExceptionDemo::throwInt() {
    throw 42;
}

void DeathTestDemo::abortProgram() {
    std::abort();
}

void DeathTestDemo::exitWithCode(int code) {
    std::exit(code);
}

void DeathTestDemo::accessNullPointer() {
    int* ptr = nullptr;
    *ptr = 42;  // 这会导致段错误
}

void DeathTestDemo::divideByZero() {
    volatile int x = 0;
    volatile int y = 1 / x;  // 除以零
    (void)y;  // 避免未使用变量警告
}

