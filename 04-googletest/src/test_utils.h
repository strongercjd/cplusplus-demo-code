#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <string>
#include <vector>
#include <stdexcept>
#include <memory>

// 用于演示测试夹具的类
class Counter {
public:
    Counter() : count_(0) {}
    void increment() { count_++; }
    void decrement() { count_--; }
    void reset() { count_ = 0; }
    int get() const { return count_; }
    
private:
    int count_;
};

// 用于演示字符串操作的类
class StringProcessor {
public:
    static std::string toUpper(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string reverse(const std::string& str);
    static bool isEmpty(const std::string& str);
    static size_t length(const std::string& str);
};

// 用于演示异常测试的函数
class ExceptionDemo {
public:
    static void throwRuntimeError();
    static void throwInvalidArgument();
    static void throwLogicError();
    static void noThrow();
    static void throwInt();
};

// 用于演示死亡测试的函数
class DeathTestDemo {
public:
    static void abortProgram();
    static void exitWithCode(int code);
    static void accessNullPointer();
    static void divideByZero();
};

// 用于演示类型化测试的模板类
template<typename T>
class Stack {
public:
    void push(const T& item) {
        items_.push_back(item);
    }
    
    T pop() {
        if (items_.empty()) {
            throw std::runtime_error("Stack is empty");
        }
        T item = items_.back();
        items_.pop_back();
        return item;
    }
    
    bool empty() const {
        return items_.empty();
    }
    
    size_t size() const {
        return items_.size();
    }
    
    T top() const {
        if (items_.empty()) {
            throw std::runtime_error("Stack is empty");
        }
        return items_.back();
    }
    
private:
    std::vector<T> items_;
};

// 用于演示自定义匹配器的类
class Person {
public:
    Person(const std::string& name, int age) : name_(name), age_(age) {}
    std::string getName() const { return name_; }
    int getAge() const { return age_; }
    
private:
    std::string name_;
    int age_;
};

#endif // TEST_UTILS_H

