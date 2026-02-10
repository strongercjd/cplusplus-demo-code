/*
 * 递归栈回溯测试
 */

#include "backward.hpp"
#include <cstdio>
#include <iostream>

using namespace backward;

typedef StackTrace stacktrace_t;

void end_of_our_journey(stacktrace_t &st) {
  if (!st.size()) {
    st.load_here();
  }
}

int rec(stacktrace_t &st, int level) {
  if (level <= 1) {
    end_of_our_journey(st);
    return 0;
  }
  return rec(st, level - 1);
}

namespace toto {

namespace titi {

struct foo {

  union bar {
    NOINLINE static int trampoline(stacktrace_t &st, int level) {
      return rec(st, level);
    }
  };
};

} // namespace titi

} // namespace toto

int fib(StackTrace &st, int level) {
  if (level == 2) {
    return 1;
  }
  if (level <= 1) {
    end_of_our_journey(st);
    return 0;
  }
  return fib(st, level - 1) + fib(st, level - 2);
}

int main() {
  stacktrace_t st;

  /*
  测试“单链递归”的栈 侧重“带复杂符号的线性递归 + 打开 object 的打印”。
  1. 调用的是 线性递归 rec(st, level)：rec(3) → rec(2) → rec(1)，单链往下走。
  2. 在 level <= 1 时调用 end_of_our_journey(st) 并在那时加载栈。*/
  std::cout << "--- rec recursive ---" << std::endl;
  const int input = 3;
  int r = toto::titi::foo::bar::trampoline(st, input);

  std::cout << "rec(" << input << ") == " << r << std::endl;

  Printer printer;
  printer.object = true; // 会多打印与可执行文件/对象相关的信息。
  printer.print(st, stdout);

  /*
  测“多分支递归”的栈，侧重“斐波那契式多分支递归 + 默认打印”，验证在更深、更复杂的调用树下栈回溯是否正常。
  1. 调用的是 斐波那契式递归 fib(st, level)：每个 fib(n) 会再调 fib(n-1) 和
  fib(n-2)，形成一棵递归树。
  2. 在 level <= 1 或 level == 2 的某一分支里调用 end_of_our_journey(st)
  并加载栈。*/

  std::cout << "--- fib recursive ---" << std::endl;
  const int input2 = 6;
  int r2 = fib(st, input2);

  std::cout << "fib(" << input2 << ") == " << r2 << std::endl;

  Printer printer2;
  printer2.print(st, stdout);

  return 0;
}