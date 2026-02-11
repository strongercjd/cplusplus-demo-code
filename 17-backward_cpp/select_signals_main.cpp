#include "backward.hpp"
#include <cstdio>
#include <iostream>

using namespace backward;

typedef StackTrace stacktrace_t;

void badass_function()
{
    char *ptr = (char *)42;
    *ptr = 42;
}

int main()
{
    std::cout << "--- select signals ---" << std::endl;
    std::vector<int> signals;
    signals.push_back(SIGSEGV);
    SignalHandling sh(signals);
    std::cout << std::boolalpha << "sh.loaded() == " << sh.loaded() << std::endl;
    badass_function();

    return 0;
}