# readme

> 17-backward_cpp的精简版本

代码解读

## 全局安装信号处理,`SignalHandling sh` 在 `main()` 之前构造

```cpp
namespace backward {

backward::SignalHandling sh;

} // namespace backward
```

## SignalHandling 构造：注册信号 + 替代栈

### 要捕获的信号

SIGSEGV（段错误）、SIGABRT（abort）、SIGFPE（除零）等，都会交给后面的 handler 处理。

```cpp
  static std::vector<int> make_default_signals() {
    const int posix_signals[] = {
      // Signals for which the default action is "Core".
      SIGABRT, // Abort signal from abort(3)
      SIGBUS,  // Bus error (bad memory access)
      SIGFPE,  // Floating point exception
      SIGILL,  // Illegal Instruction
      SIGIOT,  // IOT trap. A synonym for SIGABRT
      SIGQUIT, // Quit from keyboard
      SIGSEGV, // Invalid memory reference
      ...
    };
    return std::vector<int>(posix_signals, ...);
  }
```

### 分配并设置替代信号栈

在SignalHandling的构造函数中

```cpp
    const size_t stack_size = 1024 * 1024 * 8;
    _stack_content.reset(static_cast<char *>(malloc(stack_size)));
    if (_stack_content) {
      stack_t ss;
      ss.ss_sp = _stack_content.get();
      ss.ss_size = stack_size;
      ss.ss_flags = 0;
      if (sigaltstack(&ss, nullptr) < 0) {
        success = false;
      }
    }
```

- 用 `malloc` 分配 8MB 作为“备用栈”。
- `sigaltstack(&ss, nullptr)` 把这块内存注册为**替代信号栈**。
- 之后信号发生时，内核可以在**这块独立栈**上执行 handler，所以即使当时线程栈已经坏了（例如栈溢出），handler 仍能安全运行。

### 对每个信号调用 sigaction

```cpp
    for (size_t i = 0; i < posix_signals.size(); ++i) {
      struct sigaction action;
      memset(&action, 0, sizeof action);
      action.sa_flags =
          static_cast<int>(SA_SIGINFO | SA_ONSTACK | SA_NODEFER | SA_RESETHAND);
      sigfillset(&action.sa_mask);
      sigdelset(&action.sa_mask, posix_signals[i]);
      ...
      action.sa_sigaction = &sig_handler;
      ...
      int r = sigaction(posix_signals[i], &action, nullptr);
      ...
    }
```
- **SA_SIGINFO**：使用三参数 handler `(int, siginfo_t*, void* ucontext)`，能拿到 `siginfo_t` 和 `ucontext_t`。
- **SA_ONSTACK**：在刚才注册的替代栈上跑 handler。
- **SA_NODEFER**：执行 handler 时该信号不再被屏蔽，便于在 handler 里再 `raise`。
- **SA_RESETHAND**：进入 handler 后恢复该信号的默认处理方式。
- **action.sa_sigaction = &sig_handler**：所有上述信号的统一入口是 `sig_handler`。

这样，一旦发生段错误、abort、除零等，都会先跳到 `sig_handler`，并且能拿到“崩溃那一刻”的 CPU 上下文。

## 崩溃发生时：sig_handler → handleSignal

### 统一入口 sig_handler

```cpp
  static void
  sig_handler(int signo, siginfo_t *info, void *_ctx) {
    handleSignal(signo, info, _ctx);

    // try to forward the signal.
    raise(info->si_signo);

    // terminate the process immediately.
    puts("watf? exit");
    _exit(EXIT_FAILURE);
  }
```

流程：先做“打印栈”的完整逻辑（在 `handleSignal` 里），然后 `raise` 再发一次信号（若默认是终止则进程会按信号终止），最后 `_exit` 保证进程一定退出。

### handleSignal：从 ucontext 取“崩溃点 PC”

```cpp
  static void handleSignal(int, siginfo_t *info, void *_ctx) {
    ucontext_t *uctx = static_cast<ucontext_t *>(_ctx);

    StackTrace st;
    void *error_addr = nullptr;
#ifdef REG_RIP // x86_64
    error_addr = reinterpret_cast<void *>(uctx->uc_mcontext.gregs[REG_RIP]);
#elif defined(REG_EIP) // x86_32
    error_addr = reinterpret_cast<void *>(uctx->uc_mcontext.gregs[REG_EIP]);
#elif defined(__arm__)
    error_addr = reinterpret_cast<void *>(uctx->uc_mcontext.arm_pc);
...
#endif
    if (error_addr) {
      st.load_from(error_addr, 32, reinterpret_cast<void *>(uctx),
                   info->si_addr);
    } else {
      st.load_here(32, reinterpret_cast<void *>(uctx), info->si_addr);
    }

    Printer printer;
    printer.address = true;
    printer.print(st, stderr);
    ...
  }
```

- `_ctx` 就是内核在发信号时填好的 **ucontext_t**，里面是崩溃瞬间的寄存器快照。
- 按架构取“当前指令地址”：x86_64 用 `REG_RIP`，x86 用 `REG_EIP`，ARM 用 `arm_pc` 等，得到 **error_addr**（崩溃点 PC）。
- 若有 `error_addr`，调用 **`st.load_from(error_addr, 32, uctx, info->si_addr)`**，否则 **`st.load_here(32, uctx, info->si_addr)`**。  
  含义：以“崩溃点”为起点、最多 32 帧、并传入 ucontext 和 fault 地址，把调用栈填进 `StackTrace st`。
- 然后用 **`Printer`** 把 `st` 打到 `stderr`。

所以“捕获死机”在代码上的体现就是：**在 handleSignal 里用 ucontext 拿到 PC，再交给 StackTrace 和 Printer 做“栈回溯 + 符号解析 + 打印”。**

## 栈回溯：StackTrace::load_from / load_here

backward 在 Linux 上可能用三种实现之一：默认是 **BACKWARD_HAS_UNWIND**（`_Unwind_Backtrace`），若定义了 **BACKWARD_HAS_LIBUNWIND** 则用 libunwind。下面分两种说明。

### 方式 A：默认 Unwind（BACKWARD_HAS_UNWIND）

```cpp
  size_t load_here(size_t depth = 32, void *context = nullptr,
                   void *error_addr = nullptr) {
    load_thread_info();
    set_context(context);
    set_error_addr(error_addr);
    if (depth == 0) {
      return 0;
    }
    _stacktrace.resize(depth);
    size_t trace_cnt = details::unwind(callback(*this), depth);
    _stacktrace.resize(trace_cnt);
    skip_n_firsts(0);
    return size();
  }
```

- 这里“当前”在信号场景下是 **handler 的调用栈**（handleSignal → load_from → load_here → …）。
- **details::unwind** 内部会调用 **`_Unwind_Backtrace`**，从当前栈帧一路往回走，每帧通过 callback 把“指令地址”写进 `_stacktrace[]`。

Unwind 内部：_Unwind_Backtrace + 每帧回调

```cpp
  size_t operator()(F &f, size_t depth) {
    _f = &f;
    _index = -1;
    _depth = depth;
    _Unwind_Backtrace(&this->backtrace_trampoline, this);
    ...
  }
  ...
  _Unwind_Reason_Code backtrace(_Unwind_Context *ctx) {
    if (_index >= 0 && static_cast<size_t>(_index) >= _depth)
      return _URC_END_OF_STACK;

    int ip_before_instruction = 0;
    uintptr_t ip = _Unwind_GetIPInfo(ctx, &ip_before_instruction);
    ...
    if (_index >= 0) { // ignore first frame.
      (*_f)(static_cast<size_t>(_index), reinterpret_cast<void *>(ip));
    }
    _index += 1;
    return _URC_NO_REASON;
  }
```

- `_Unwind_Backtrace` 会按栈帧依次调用 `backtrace_trampoline` → `backtrace`。
- 每帧用 **`_Unwind_GetIPInfo`** 取出该帧的 IP，通过 **`(*_f)(index, (void*)ip)`** 写进 `_stacktrace[index]`。  
  也就是说：**栈里存的是一串“调用点”的指令地址。**

load_from：从“崩溃帧”开始截断（883–895 行）

```cpp
  size_t load_from(void *addr, size_t depth = 32, void *context = nullptr,
                   void *error_addr = nullptr) {
    load_here(depth + 8, context, error_addr);

    for (size_t i = 0; i < _stacktrace.size(); ++i) {
      if (_stacktrace[i] == addr) {
        skip_n_firsts(i);
        break;
      }
    }

    _stacktrace.resize(std::min(_stacktrace.size(), skip_n_firsts() + depth));
    return size();
  }
```

- 先 **load_here(depth+8, context, error_addr)**，得到“从 handler 开始一直到 main 甚至更早”的完整回溯（多要 8 帧是为了确保能覆盖到崩溃点）。
- 然后**在 _stacktrace 里找第一个等于 `addr` 的帧**（`addr` 就是 handleSignal 里传下来的 **error_addr**，即崩溃点 PC）。
- **skip_n_firsts(i)** 表示“前面 i 帧都跳过”，所以对外 **size()** 和 **operator[]** 只暴露“从崩溃点开始”的栈。
- 再 resize 成最多 `skip_n_firsts() + depth` 帧。

因此：**在信号 handler 里用 Unwind 时，栈是从“当前”（handler）开始展开的，再通过 load_from(error_addr) 在数组里找到崩溃帧，只保留“崩溃点及以后”的栈用于打印。**

### 方式 B：libunwind（BACKWARD_HAS_LIBUNWIND）

若在 `backward.cpp` 里定义了 `BACKWARD_HAS_LIBUNWIND`，会用另一套 `StackTraceImpl`，其 **load_here** 会真正用到 **context（ucontext_t）**：

```934:946:backward.hpp
    if (context()) {
      ucontext_t *uctx = reinterpret_cast<ucontext_t *>(context());
#ifdef REG_RIP         // x86_64
      if (uctx->uc_mcontext.gregs[REG_RIP] ==
          reinterpret_cast<greg_t>(error_addr())) {
        uctx->uc_mcontext.gregs[REG_RIP] =
            *reinterpret_cast<size_t *>(uctx->uc_mcontext.gregs[REG_RSP]);
      }
      _stacktrace[index] =
          reinterpret_cast<void *>(uctx->uc_mcontext.gregs[REG_RIP]);
      ++index;
      ctx = *reinterpret_cast<unw_context_t *>(uctx);
```

- 从 **ucontext_t** 里取出 RIP（或 EIP/PC），先写入第一帧 `_stacktrace[0]`。
- 若 RIP 正好等于 fault 地址（坏函数指针调用），会用栈上的返回地址替换 RIP，避免错误的一帧。
- 然后把 ucontext 转成 libunwind 的 **unw_context_t**，后面用 **unw_init_local** + **unw_step** 从这一帧开始往后 unwind，每帧用 **unw_get_reg(UNW_REG_IP)** 得到 IP 填入 `_stacktrace[index]`。

也就是说：**用 libunwind 时，栈是“从崩溃瞬间的 CPU 上下文”开始、一帧一帧往后走的，更适合在信号 handler 里精确还原崩溃时的调用栈。**

## 符号解析：TraceResolver::load_stacktrace + resolve

栈里现在是一串 **void*** 地址，要变成“文件名、函数名、行号”等，由 **TraceResolver** 完成。

### 栈帧的数据结构

```cpp
struct Trace {
  void *addr;
  size_t idx;
  ...
};

struct ResolvedTrace : public Trace {
  struct SourceLoc {
    std::string function;
    std::string filename;
    unsigned line;
    unsigned col;
    ...
  };
  std::string object_filename;   // 二进制路径
  std::string object_function;  // 符号名（可 demangle）
  SourceLoc source;             // 源文件、行号、列、函数
  source_locs_t inliners;       // 内联链
  ...
};
```

- **Trace**：只有 `addr`（指令地址）和 `idx`（帧序号）。
- **ResolvedTrace**：在 Trace 基础上加上“在哪个 so/可执行里、哪个符号、源文件行号、内联信息”等。

### Printer 如何驱动解析

```cpp
  void print_stacktrace(ST &st, std::ostream &os, Colorize &colorize) {
    print_header(os, st.thread_id());
    _resolver.load_stacktrace(st);
    if ( reverse ) {
      for (size_t trace_idx = st.size(); trace_idx > 0; --trace_idx) {
        print_trace(os, _resolver.resolve(st[trace_idx - 1]), colorize);
      }
    } else {
      for (size_t trace_idx = 0; trace_idx < st.size(); ++trace_idx) {
        print_trace(os, _resolver.resolve(st[trace_idx]), colorize);
      }
    }
  }
```

- **st[i]** 是 **Trace**：`Trace(addr, idx)`，来自 `StackTraceImplHolder::operator[]`（785–790 行），即 `Trace(_stacktrace[idx + skip_n_firsts()], idx)`。  
- **load_stacktrace(st)** 里会调 **load_addresses(st.begin(), st.size())**，把整条栈的地址交给 resolver 做“批量准备”（例如 backtrace_symbols 一次解析整条栈）。  
- 对每一帧 **st[trace_idx]**，先 **resolve(该 Trace)** 得到 **ResolvedTrace**，再 **print_trace** 打印。  

### 默认解析器：backtrace_symbols

未定义 BACKWARD_HAS_DW/BFD/DWARF 时，用的是 **backtrace_symbol** 这一套：

```cpp
  void load_addresses(void *const*addresses, int address_count) override {
    if (address_count == 0) {
      return;
    }
    _symbols.reset(backtrace_symbols(addresses, address_count));
  }

  ResolvedTrace resolve(ResolvedTrace trace) override {
    char *filename = _symbols[trace.idx];
    char *funcname = filename;
    while (*funcname && *funcname != '(') {
      funcname += 1;
    }
    trace.object_filename.assign(filename, funcname);
    if (*funcname) {
      funcname += 1;
      char *funcname_end = funcname;
      while (*funcname_end && *funcname_end != ')' && *funcname_end != '+') {
        funcname_end += 1;
      }
      *funcname_end = '\0';
      trace.object_function = this->demangle(funcname);
      trace.source.function = trace.object_function;
    }
    return trace;
  }
```

- **load_addresses**：对整条栈调用 **backtrace_symbols(addresses, address_count)**，得到一串 "path(func+offset)" 的字符串数组。  
- **resolve**：用 **trace.idx** 取第 idx 帧的字符串，解析出“对象路径”和“括号里的函数名”，再 **demangle** 成可读 C++ 名，填进 **object_filename / object_function**。  
  默认没有 DWARF，所以 **source.filename / line** 为空，只有“对象 + 函数名”。  

若在 backward.cpp 里打开 **BACKWARD_HAS_DW**，会改用 libdw 的 TraceResolver，通过 DWARF 查 **source.filename、line、col** 甚至内联链，逻辑在 1814 行附近的 **resolve** 里（dwfl_module_addrdie、find_fundie_by_pc 等），这里不逐行展开，但流程同样是：**addr → 所属模块 → 该地址对应的源位置**。

## 打印：print_trace / print_source_loc / print_snippet

### 单帧打印

```cpp
  void print_trace(std::ostream &os, const ResolvedTrace &trace,
                   Colorize &colorize) {
    os << "#" << std::left << std::setw(2) << trace.idx << std::right;
    bool already_indented = true;

    if (!trace.source.filename.size() || object) {
      os << "   Object \"" << trace.object_filename << "\", at " << trace.addr
         << ", in " << trace.object_function << "\n";
      already_indented = false;
    }

    for (size_t inliner_idx = trace.inliners.size(); inliner_idx > 0;
         --inliner_idx) {
      ...
      print_source_loc(os, " | ", inliner_loc);
      if (snippet) {
        print_snippet(os, "    | ", inliner_loc, colorize, Color::purple, ...);
      }
      ...
    }

    if (trace.source.filename.size()) {
      ...
      print_source_loc(os, "   ", trace.source, trace.addr);
      if (snippet) {
        print_snippet(os, "      ", trace.source, colorize, Color::yellow, ...);
      }
    }
  }
```

- 先打 **#idx**。
- 若没有源信息或强制只看 object，就打 **Object "path", at addr, in function**。
- 若有 **inliners**（内联链），对每一层调用 **print_source_loc**，可选 **print_snippet** 打源码片段。
- 最后若有 **trace.source**，再打一层 **Source "file", line N, in function**，同样可选 snippet。

### 源位置一行

```cpp
  void print_source_loc(std::ostream &os, const char *indent,
                        const ResolvedTrace::SourceLoc &source_loc,
                        void *addr = nullptr) {
    os << indent << "Source \"" << source_loc.filename << "\", line "
       << source_loc.line << ", in " << source_loc.function;

    if (address && addr != nullptr) {
      os << " [" << addr << "]";
    }
    os << "\n";
  }
```

handleSignal 里设置了 **printer.address = true**，所以会带上 `[addr]`。整体效果就是你看到的 “#0 … Object … at 0x… in …” 和 “Source “…”, line N, in … [0x…]”。
