#include "backward.hpp"
#include <cstdio>
#include <iostream>

using namespace backward;

typedef StackTrace stacktrace_t;

void collect_trace(StackTrace &st) { st.load_here(); }

void d(StackTrace &st) { st.load_here(); }

void c(StackTrace &st) { return d(st); }

void b(StackTrace &st) { return c(st); }

NOINLINE void a(StackTrace &st) { return b(st); }

int main()
{
    std::cout << "--- stacktrace ---" << std::endl;
    Printer printer;

    StackTrace st;
    a(st);

    printer.print(st, std::cout);
}