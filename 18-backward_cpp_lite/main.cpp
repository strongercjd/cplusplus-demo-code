#include "backward.hpp"
#include <cstdio>
#include <iostream>
#include <sys/resource.h>
#include <cstdlib>

using namespace backward;

typedef StackTrace stacktrace_t;

void badass_function()
{
    char *ptr = (char *)42;
    *ptr = 42;
}

int you_shall_not_pass()
{
    char *ptr = (char *)42;
    int v = *ptr;
    return v;
}

void abort_abort_I_repeat_abort_abort()
{
    std::cout << "Jumping off the boat!" << std::endl;
    abort();
}
volatile int zero = 0;
int divide_by_zero()
{
    std::cout << "And the wild black hole appears..." << std::endl;
    int v = 42 / zero;
    return v;
}

int bye_bye_stack(int i) { return bye_bye_stack(i + 1) + bye_bye_stack(i * 2); }

int main(int argc, char *argv[])
{
    auto print_help = [&]()
    {
        std::cout << "Usage: " << (argc >= 1 ? argv[0] : "suicide_main") << " <test_id>\n"
                  << "  test_id | test name        | description\n"
                  << "  --------|------------------|----------------\n"
                  << "  0       | invalid_write    | 非法写内存\n"
                  << "  1       | invalid_read     | 非法读内存\n"
                  << "  2       | abort            | 调用 abort()\n"
                  << "  3       | divide_by_zero   | 除零\n"
                  << "  4       | stackoverflow    | 栈溢出\n";
    };

    if (argc < 2)
    {
        print_help();
        return 0;
    }

    int id = std::atoi(argv[1]);
    std::cout << "--- suicide (test " << id << ") ---" << std::endl;

    switch (id)
    {
    case 0:
        std::cout << "!!!!invalid_write!!!!" << std::endl;
        badass_function();
        break;
    case 1:
    {
        std::cout << "!!!!invalid_read!!!!" << std::endl;
        int v = you_shall_not_pass();
        std::cout << "v=" << v << std::endl;
        break;
    }
    case 2:
        std::cout << "!!!!abort!!!!" << std::endl;
        abort_abort_I_repeat_abort_abort();
        break;
    case 3:
    {
        std::cout << "!!!!divide_by_zero!!!!" << std::endl;
        int v = divide_by_zero();
        std::cout << "v=" << v << std::endl;
        break;
    }
    case 4:
    {
        std::cout << "!!!!stackoverflow!!!!" << std::endl;
        struct rlimit limit;
        limit.rlim_max = 8096;
        setrlimit(RLIMIT_STACK, &limit);
        int r = bye_bye_stack(42);
        std::cout << "r=" << r << std::endl;
        break;
    }
    default:
        std::cerr << "Unknown test id: " << id << std::endl;
        print_help();
        return 1;
    }
    return 0;
}