# readme

栈帧编号越大越“老”（更早的调用），#0 是崩溃发生时的最顶层。

Printer 里能用来“配置”打印行为

| 成员 | 类型 | 默认值 | 作用 |
|------|------|--------|------|
| **snippet** | `bool` | `true` | 是否打印源码片段（出错行前后几行） |
| **color_mode** | `ColorMode::type` | `ColorMode::automatic` | 输出颜色模式（自动/开/关） |
| **address** | `bool` | `false` | 是否在源码行后显示指令地址 `[addr]` |
| **object** | `bool` | `false` | 是否总是打印 Object 信息（二进制路径 + 地址 + 符号） |
| **inliner_context_size** | `int` | `5` | 内联帧源码片段的上下文行数（前后各几行） |
| **trace_context_size** | `int` | `7` | 主栈帧源码片段的上下文行数 |
| **reverse** | `bool` | `true` | 栈帧顺序：`true` 为“最晚调用在前”（常见崩溃打印顺序），`false` 为从 main 往上的顺序 |

## rectrace_main.cpp

栈回朔示例代码。

```c++
  std::cout << "--- rec recursive ---" << std::endl;
  const int input = 3;
  int r = toto::titi::foo::bar::trampoline(st, input);

  std::cout << "rec(" << input << ") == " << r << std::endl;

  Printer printer;
  printer.object = true; // 会多打印与可执行文件/对象相关的信息。
  printer.print(st, stdout);
```

测试“单链递归”的栈 侧重“带复杂符号的线性递归 + 打开 object 的打印”。  
1. 调用的是 线性递归 rec(st, level)：rec(3) → rec(2) → rec(1)，单链往下走。   
2. 在 level <= 1 时调用 end_of_our_journey(st) 并在那时加载栈。  

```c++
  std::cout << "--- fib recursive ---" << std::endl;
  const int input2 = 6;
  int r2 = fib(st, input2);

  std::cout << "fib(" << input2 << ") == " << r2 << std::endl;

  Printer printer2;
  printer2.print(st, stdout);
```

测“多分支递归”的栈，侧重“斐波那契式多分支递归 + 默认打印”，验证在更深、更复杂的调用树下栈回溯是否正常。  
1. 调用的是 斐波那契式递归 fib(st, level)：每个 fib(n) 会再调 fib(n-1) 和fib(n-2)，形成一棵递归树。  
2. 在 level <= 1 或 level == 2 的某一分支里调用 end_of_our_journey(st)并加载栈。

运行结果

```bash
$ ./rectrace_main 
--- rec recursive ---
rec(3) == 0
Stack trace (most recent call last):
#11   Object "[0xffffffffffffffff]", at 0xffffffffffffffff, in 
#10   Object "./rectrace_main", at 0x5558d59d5b44, in 
#9    Object "/lib/x86_64-linux-gnu/libc.so.6", at 0x74f7b522a28a, in __libc_start_main
#8    Object "/lib/x86_64-linux-gnu/libc.so.6", at 0x74f7b522a1c9, in 
#7    Object "./rectrace_main", at 0x5558d59d5d72, in 
#6    Object "./rectrace_main", at 0x5558d59d9050, in 
#5    Object "./rectrace_main", at 0x5558d59d5c8c, in 
#4    Object "./rectrace_main", at 0x5558d59d5c8c, in 
#3    Object "./rectrace_main", at 0x5558d59d5c71, in 
#2    Object "./rectrace_main", at 0x5558d59d5c49, in 
#1    Object "./rectrace_main", at 0x5558d59d6afd, in 
#0    Object "./rectrace_main", at 0x5558d59d9579, in 
--- fib recursive ---
fib(6) == 5
Stack trace (most recent call last):
#11   Object "[0xffffffffffffffff]", at 0xffffffffffffffff, in 
#10   Object "./rectrace_main", at 0x5558d59d5b44, in 
#9    Object "/lib/x86_64-linux-gnu/libc.so.6", at 0x74f7b522a28a, in __libc_start_main
#8    Object "/lib/x86_64-linux-gnu/libc.so.6", at 0x74f7b522a1c9, in 
#7    Object "./rectrace_main", at 0x5558d59d5d72, in 
#6    Object "./rectrace_main", at 0x5558d59d9050, in 
#5    Object "./rectrace_main", at 0x5558d59d5c8c, in 
#4    Object "./rectrace_main", at 0x5558d59d5c8c, in 
#3    Object "./rectrace_main", at 0x5558d59d5c71, in 
#2    Object "./rectrace_main", at 0x5558d59d5c49, in 
#1    Object "./rectrace_main", at 0x5558d59d6afd, in 
#0    Object "./rectrace_main", at 0x5558d59d9579, in
```

只有位置信息，没有更具体的信息，需要安装 libdw-dev

```bash
sudo apt-get install libdw-dev
```

并且新增编译选项

```cmake
# 添加 libdw 库
target_link_libraries(rectrace_main PRIVATE dw)
# 定义 BACKWARD_HAS_DW=1 宏
target_compile_definitions(rectrace_main PRIVATE BACKWARD_HAS_DW=1)
```

重新编译运行

```bash
$ ./rectrace_main 
--- rec recursive ---
rec(3) == 0
Stack trace (most recent call last):
#11   Object "/usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2", at 0xffffffffffffffff, in 
#10   Object "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/build/rectrace_main", at 0x62faaf14cea4, in _start
#9    Object "/usr/lib/x86_64-linux-gnu/libc.so.6", at 0x70cca5a2a28a, in __libc_start_main@@GLIBC_2.34
      Source "../csu/libc-start.c", line 360, in __libc_start_main_impl
#8    Object "/usr/lib/x86_64-linux-gnu/libc.so.6", at 0x70cca5a2a1c9, in __libc_start_call_main
      Source "../sysdeps/nptl/libc_start_call_main.h", line 58, in __libc_start_call_main
#7    Object "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/build/rectrace_main", at 0x62faaf14d0d2, in main
      Source "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/rectrace_main.cpp", line 64, in main
         61:   2. 在 level <= 1 时调用 end_of_our_journey(st) 并在那时加载栈。*/
         62:   std::cout << "--- rec recursive ---" << std::endl;
         63:   const int input = 3;
      >  64:   int r = toto::titi::foo::bar::trampoline(st, input);
         65: 
         66:   std::cout << "rec(" << input << ") == " << r << std::endl;
#6    Object "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/build/rectrace_main", at 0x62faaf150dc2, in toto::titi::foo::bar::trampoline(backward::StackTrace&, int)
      Source "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/rectrace_main.cpp", line 35, in trampoline
         33:   union bar {
         34:     NOINLINE static int trampoline(stacktrace_t &st, int level) {
      >  35:       return rec(st, level);
         36:     }
         37:   };
         38: };
#5    Object "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/build/rectrace_main", at 0x62faaf14cfec, in rec(backward::StackTrace&, int)
      Source "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/rectrace_main.cpp", line 24, in rec
         21:     end_of_our_journey(st);
         22:     return 0;
         23:   }
      >  24:   return rec(st, level - 1);
         25: }
         26: 
         27: namespace toto {
#4    Object "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/build/rectrace_main", at 0x62faaf14cfec, in rec(backward::StackTrace&, int)
      Source "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/rectrace_main.cpp", line 24, in rec
         21:     end_of_our_journey(st);
         22:     return 0;
         23:   }
      >  24:   return rec(st, level - 1);
         25: }
         26: 
         27: namespace toto {
#3    Object "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/build/rectrace_main", at 0x62faaf14cfd1, in rec(backward::StackTrace&, int)
      Source "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/rectrace_main.cpp", line 21, in rec
         19: int rec(stacktrace_t &st, int level) {
         20:   if (level <= 1) {
      >  21:     end_of_our_journey(st);
         22:     return 0;
         23:   }
         24:   return rec(st, level - 1);
#2    Object "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/build/rectrace_main", at 0x62faaf14cfa9, in end_of_our_journey(backward::StackTrace&)
      Source "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/rectrace_main.cpp", line 15, in end_of_our_journey
         13: void end_of_our_journey(stacktrace_t &st) {
         14:   if (!st.size()) {
      >  15:     st.load_here();
         16:   }
         17: }
#1    Object "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/build/rectrace_main", at 0x62faaf14de5d, in backward::StackTraceImpl<backward::system_tag::linux_tag>::load_here(unsigned long, void*, void*)
      Source "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/backward.hpp", line 879, in load_here
        876:       return 0;
        877:     }
        878:     _stacktrace.resize(depth);
      > 879:     size_t trace_cnt = details::unwind(callback(*this), depth);
        880:     _stacktrace.resize(trace_cnt);
        881:     skip_n_firsts(0);
        882:     return size();
#0    Object "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/build/rectrace_main", at 0x62faaf1512eb, in unsigned long backward::details::unwind<backward::StackTraceImpl<backward::system_tag::linux_tag>::callback>(backward::StackTraceImpl<backward::system_tag::linux_tag>::callback, unsigned long)
      Source "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/backward.hpp", line 861, in unwind<backward::StackTraceImpl<backward::system_tag::linux_tag>::callback>
        859: template <typename F> size_t unwind(F f, size_t depth) {
        860:   Unwinder<F> unwinder;
      > 861:   return unwinder(f, depth);
        862: }
        863: 
        864: } // namespace details
--- fib recursive ---
fib(6) == 5
Stack trace (most recent call last):
#11   Object "/usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2", at 0xffffffffffffffff, in 
#10   Object "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/build/rectrace_main", at 0x62faaf14cea4, in _start
#9    Source "../csu/libc-start.c", line 360, in __libc_start_main_impl
#8    Source "../sysdeps/nptl/libc_start_call_main.h", line 58, in __libc_start_call_main
#7    Source "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/rectrace_main.cpp", line 64, in main
         61:   2. 在 level <= 1 时调用 end_of_our_journey(st) 并在那时加载栈。*/
         62:   std::cout << "--- rec recursive ---" << std::endl;
         63:   const int input = 3;
      >  64:   int r = toto::titi::foo::bar::trampoline(st, input);
         65: 
         66:   std::cout << "rec(" << input << ") == " << r << std::endl;
#6    Source "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/rectrace_main.cpp", line 35, in trampoline
         33:   union bar {
         34:     NOINLINE static int trampoline(stacktrace_t &st, int level) {
      >  35:       return rec(st, level);
         36:     }
         37:   };
         38: };
#5    Source "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/rectrace_main.cpp", line 24, in rec
         21:     end_of_our_journey(st);
         22:     return 0;
         23:   }
      >  24:   return rec(st, level - 1);
         25: }
         26: 
         27: namespace toto {
#4    Source "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/rectrace_main.cpp", line 24, in rec
         21:     end_of_our_journey(st);
         22:     return 0;
         23:   }
      >  24:   return rec(st, level - 1);
         25: }
         26: 
         27: namespace toto {
#3    Source "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/rectrace_main.cpp", line 21, in rec
         19: int rec(stacktrace_t &st, int level) {
         20:   if (level <= 1) {
      >  21:     end_of_our_journey(st);
         22:     return 0;
         23:   }
         24:   return rec(st, level - 1);
#2    Source "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/rectrace_main.cpp", line 15, in end_of_our_journey
         13: void end_of_our_journey(stacktrace_t &st) {
         14:   if (!st.size()) {
      >  15:     st.load_here();
         16:   }
         17: }
#1    Source "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/backward.hpp", line 879, in load_here
        876:       return 0;
        877:     }
        878:     _stacktrace.resize(depth);
      > 879:     size_t trace_cnt = details::unwind(callback(*this), depth);
        880:     _stacktrace.resize(trace_cnt);
        881:     skip_n_firsts(0);
        882:     return size();
#0    Source "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/backward.hpp", line 861, in unwind<backward::StackTraceImpl<backward::system_tag::linux_tag>::callback>
        859: template <typename F> size_t unwind(F f, size_t depth) {
        860:   Unwinder<F> unwinder;
      > 861:   return unwinder(f, depth);
        862: }
        863: 
        864: } // namespace details
```

关于object

```cpp
printer.object = true;
```

如果为true，就会打印如下,存在Object

```bash
#7    Object "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/build/rectrace_main", at 0x62faaf14d0d2, in main
      Source "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/rectrace_main.cpp", line 64, in main
         61:   2. 在 level <= 1 时调用 end_of_our_journey(st) 并在那时加载栈。*/
         62:   std::cout << "--- rec recursive ---" << std::endl;
         63:   const int input = 3;
      >  64:   int r = toto::titi::foo::bar::trampoline(st, input);
         65: 
         66:   std::cout << "rec(" << input << ") == " << r << std::endl;
```

如果为false表示，就不会输出Object

```bash
#7    Source "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/rectrace_main.cpp", line 64, in main
         61:   2. 在 level <= 1 时调用 end_of_our_journey(st) 并在那时加载栈。*/
         62:   std::cout << "--- rec recursive ---" << std::endl;
         63:   const int input = 3;
      >  64:   int r = toto::titi::foo::bar::trampoline(st, input);
         65: 
         66:   std::cout << "rec(" << input << ") == " << r << std::endl;
```

当然有些没有源码位置的，还是会输出 Object

```bash
#11   Object "/usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2", at 0xffffffffffffffff, in 
#10   Object "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/build/rectrace_main", at 0x5b5750bdeea4, in _start
```


## select_signals_main.cpp

捕获段错误，其中 SIGSEGV 定义在 /usr/include/x86_64-linux-gnu/bits/signum-generic.h (ubuntu2404)

```C
/* ISO C99 signals.  */
#define	SIGINT		2	/* Interactive attention signal.  */
#define	SIGILL		4	/* Illegal instruction.  */
#define	SIGABRT		6	/* Abnormal termination.  */
#define	SIGFPE		8	/* Erroneous arithmetic operation.  */
#define	SIGSEGV		11	/* Invalid access to storage.  */
#define	SIGTERM		15	/* Termination request.  */

/* Historical signals specified by POSIX. */
#define	SIGHUP		1	/* Hangup.  */
#define	SIGQUIT		3	/* Quit.  */
#define	SIGTRAP		5	/* Trace/breakpoint trap.  */
#define	SIGKILL		9	/* Killed.  */
#define	SIGPIPE		13	/* Broken pipe.  */
#define	SIGALRM		14	/* Alarm clock.  */
```

这段头文件定义的是 **Linux/Unix 标准信号（signal）的宏**，每个宏对应一个整数编号和简短说明。下面按“用途 + 典型触发场景”简要解读。

---

### 一、ISO C99 标准信号（可移植）

| 宏 | 编号 | 含义与典型场景 |
|----|------|----------------|
| **SIGINT** | 2 | **交互式中断**。用户按 Ctrl+C 时由终端驱动发给前台进程，请求“停下来”，进程可捕获并做清理后退出。 |
| **SIGILL** | 4 | **非法指令**。CPU 执行了无效/未定义的机器指令，常见于：损坏的可执行文件、错误的自修改代码、或在不支持某指令的 CPU 上运行。 |
| **SIGABRT** | 6 | **异常终止**。通常由程序主动调用 `abort()` 触发（如断言失败、`std::terminate` 等），表示“程序认为发生了严重错误并主动退出”。 |
| **SIGFPE** | 8 | **算术错误**。例如整数除零、浮点异常、溢出等，由 CPU 在运算时触发。 |
| **SIGSEGV** | 11 | **无效存储访问**。访问了不该访问的内存（空指针解引用、越界、非法地址等），即常说的“段错误”（Segmentation fault）。 |
| **SIGTERM** | 15 | **终止请求**。请求进程“正常退出”，例如 `kill <pid>` 默认发的就是 SIGTERM；进程应捕获并做清理后退出，区别于“强制杀死”。 |

---

### 二、POSIX 历史信号（常见于 Unix/Linux）

| 宏 | 编号 | 含义与典型场景 |
|----|------|----------------|
| **SIGHUP** | 1 | **挂断**。终端断开时发给该终端关联的会话/进程（如 SSH 断开），也常被 daemon 用来“重新加载配置”。 |
| **SIGQUIT** | 3 | **退出**。用户按 Ctrl+\ 时产生，类似 SIGINT 但默认会产生 core dump，用于调试。 |
| **SIGTRAP** | 5 | **跟踪/断点**。调试器设置断点或单步执行时由 CPU 触发，也可由 `ptrace` 等机制产生。 |
| **SIGKILL** | 9 | **杀死**。立即终止进程，**不可捕获、不可忽略**，用于强制结束进程（如 `kill -9`）。 |
| **SIGPIPE** | 13 | **管道破裂**。向已关闭读端的管道/套接字写数据时产生（如对方已关闭连接），忽略后写操作会返回 EPIPE。 |
| **SIGALRM** | 14 | **闹钟**。由 `alarm()` 或 `setitimer()` 在指定时间到达时发送，常用于超时、定时任务。 |

---

### 三、小结

- **C99 那组**：和“程序错误/异常/退出”紧密相关（崩溃、断言、除零、段错误、用户中断、请求退出）。
- **POSIX 那组**：和“终端/会话/调试/通信/定时”相关（挂断、Ctrl+\、断点、强制杀、管道、闹钟）。
- 编号在不同 Unix 系统上可能略有差异，所以代码里应始终用宏名（如 `SIGSEGV`），不要写死数字。
- 除 **SIGKILL** 和 **SIGSTOP** 外，这些信号一般都可以被进程捕获（`signal()`/`sigaction()`）或忽略，用于自定义处理或优雅退出。

准确定位到出错的位置

```bash
$ ./select_signals_main 
--- select signals ---
sh.loaded() == true
Stack trace (most recent call last):
#5    Object "/usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2", at 0xffffffffffffffff, in 
#4    Object "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/build/select_signals_main", at 0x58bb5dc07ec4, in _start
#3    Source "../csu/libc-start.c", line 360, in __libc_start_main_impl [0x751c2622a28a]
#2    Source "../sysdeps/nptl/libc_start_call_main.h", line 58, in __libc_start_call_main [0x751c2622a1c9]
#1    Source "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/select_signals_main.cpp", line 22, in main [0x58bb5dc08083]
         19:     signals.push_back(SIGSEGV);
         20:     SignalHandling sh(signals);
         21:     std::cout << std::boolalpha << "sh.loaded() == " << sh.loaded() << std::endl;
      >  22:     badass_function();
         23: 
         24:     return 0;
         25: }
#0    Source "/home/jindouchen/code/cplusplus-demo-code/17-backward_cpp/select_signals_main.cpp", line 12, in badass_function [0x58bb5dc07f9d]
          9: void badass_function()
         10: {
         11:     char *ptr = (char *)42;
      >  12:     *ptr = 42;
         13: }
         14: 
         15: int main()
Segmentation fault (Address not mapped to object [0x2a])
段错误 (核心已转储)
```

## stacktrace_main.cpp

这个和stacktrace_main.cpp的作用是一样的，都是打印栈的信息，细节区别如下


### **rectrace** — 递归 + 复杂符号

- **调用方式**：**递归**  
  - `rec(st, level)` 或 `fib(st, level)`，同一函数在栈里出现多次。
- **符号**：涉及嵌套命名空间、`struct`、`union`、静态成员等（如 `toto::titi::foo::bar::trampoline`），用来测 **demangle** 和复杂 C++ 名。
- **目的**：验证  
  1）**递归栈**下（重复的栈帧、可能较深的栈）栈回溯是否仍然正确；  
  2）**复杂 C++ 名字**能否被正确解析和打印。

---

### **stacktrace** — 简单、线性调用栈

- **调用方式**：**线性链式调用**，没有递归  
  - `a()` → `b()` → `c()` → `d()`，在 `d()` 里调 `st.load_here()`。
- **栈形状**：每一层都是不同的函数，栈帧不重复。
- **目的**：验证「在普通、简单的调用链下，backward 能否正确收集并打印栈」。

### 对比小结

| 维度       | test_stacktrace        | test_rectrace                    |
|------------|------------------------|----------------------------------|
| 调用结构   | 线性 a→b→c→d          | 递归（rec / fib）                |
| 栈帧       | 每层不同函数           | 同一函数多次出现                 |
| 符号复杂度 | 简单函数名             | 命名空间/类/union 等             |
| 侧重       | 基础「能打出栈」       | 递归 + 复杂名下的正确性/稳定性   |

所以：**stacktrace 测的是「最基础的栈回溯」**，**rectrace 测的是「在递归和复杂 C++ 符号下栈回溯是否仍然可靠」**。

## suicide main.cpp

自杀测试，主动制造一些错误，去抓取栈信息,测试都是在**主动制造**各种致命情况：

| 测试 | 行为 |
|------|------|
| `invalid_write` | 往非法地址写 → 段错误 |
| `invalid_read` | 从非法地址读 → 段错误 |
| `calling_abort` | 调用 `abort()` → 进程终止 |
| `divide_by_zero` | 除零 → 可能 SIGFPE |
| `stackoverflow` | 无限递归 + 小栈限制 → 栈溢出 |


