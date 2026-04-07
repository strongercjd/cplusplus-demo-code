#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <optional>
#include <queue>
#include <sstream>
#include <string>
#include <thread>

// 这个 demo 的核心目的是解释 “为什么要把消息处理逻辑通过 std::bind 封装成 void() 任务再投递”，从而实现线程切换、排队串行化与解耦

namespace demo
{

  static std::string tid()
  {
    std::ostringstream oss;
    oss << std::this_thread::get_id(); // 获取当前线程的ID
    return oss.str();                  // 将ID转换为字符串
  }

  static std::string now()
  {
    using namespace std::chrono;
    const auto ms = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count(); // 获取当前时间戳
    std::ostringstream oss;
    oss << ms;        // 将时间戳转换为字符串
    return oss.str(); // 返回时间戳字符串
  }

  class TaskManager
  {
  public:
    using Task = std::function<void()>;

    TaskManager() : stop_(false)
    {
      worker_ = std::thread([this]
                            { this->loop(); });
      // 等待 worker thread 启动并记录 thread id
      std::unique_lock<std::mutex> lk(mu_);
      cv_started_.wait(lk, [this]
                       { return worker_tid_.has_value(); });
    }

    ~TaskManager()
    {
      {
        std::lock_guard<std::mutex> lk(mu_);
        stop_ = true;
        cv_.notify_all();
      }
      if (worker_.joinable())
        worker_.join();
    }

    // 异步投递：排队到 worker 线程执行（模拟 postFunc）
    void postFunc(Task &&task)
    {
      std::lock_guard<std::mutex> lk(mu_);
      q_.push(std::move(task));
      cv_.notify_one();
    }

    // 直接调用：不排队，立刻在“当前线程”执行（模拟 callFunc）
    void callFunc(Task &&task) { task(); }

    std::string workerThreadId() const // 获取事件循环线程的ID
    {
      std::lock_guard<std::mutex> lk(mu_);
      return worker_tid_.value_or("<unknown>");
    }

  private:
    void loop()
    {
      {
        std::lock_guard<std::mutex> lk(mu_);
        worker_tid_ = tid();
        cv_started_.notify_all();
      }

      for (;;)
      {
        Task task;
        {
          std::unique_lock<std::mutex> lk(mu_);
          cv_.wait(lk, [this]
                   { return stop_ || !q_.empty(); });
          if (stop_ && q_.empty())
            return;
          task = std::move(q_.front());
          q_.pop();
        }
        task();
      }
    }

    mutable std::mutex mu_;
    std::condition_variable cv_;
    std::condition_variable cv_started_;
    std::queue<Task> q_;
    bool stop_;
    std::thread worker_;
    std::optional<std::string> worker_tid_;
  };

  struct JobMessage
  {
    int event = 0;
    std::string payload;
  };

  class JobManager
  {
  public:
    using DispatchMessageFunc = std::function<void(std::function<void()> &&)>;
    using ProcMessageFunc = std::function<void(JobMessage &)>;

    void registDispatchMsgCb(DispatchMessageFunc func) { dispatchMessageCb_ = std::move(func); }
    void registDirectCallMsgCb(DispatchMessageFunc func) { directCallMessageCb_ = std::move(func); }
    void registProcMessageCb(ProcMessageFunc func) { procMessageFunc_ = std::move(func); }

    void sendMessage(JobMessage &msg)
    {
      if (!dispatchMessageCb_)
      {
        log("sendMessage: dispatchMessageCb not set");
        return;
      }
      dispatchMessageCb_(std::bind(&JobManager::procMessage, this, msg)); // msg 会被拷贝进任务里
      /*
      拆解一下这个语句：
      dispatchMessageCb_(std::bind(&JobManager::procMessage, this, msg));
      1. std::bind(&JobManager::procMessage, this, msg) 会返回一个 std::function<void()>
      2. dispatchMessageCb_就是可执行对象，它接受一个 std::function<void()> 类型的参数，并调用它。

      这句代码拆开写就是：
      std::function<void()> task = std::bind(&JobManager::procMessage, this, msg);//先创建“任务”对象（无参、无返回值）
      dispatchMessageCb_(std::move(task));//再投递“任务”对象
       */
    }

    void callMessage(JobMessage &msg)
    {
      if (!directCallMessageCb_)
      {
        log("callMessage: directCallMessageCb not set");
        return;
      }
      directCallMessageCb_(std::bind(&JobManager::procMessage, this, msg));
    }

  private:
    void procMessage(JobMessage &msg)
    {
      if (procMessageFunc_)
        procMessageFunc_(msg);
    }

    static void log(const std::string &s)
    {
      std::cout << "[" << now() << "] [tid " << tid() << "] " << s << "\n";
    }

    DispatchMessageFunc dispatchMessageCb_;
    DispatchMessageFunc directCallMessageCb_;
    ProcMessageFunc procMessageFunc_;
  };

  class JobExecutorManagerBase
  {
  public:
    explicit JobExecutorManagerBase(JobManager &jm) : jm_(jm)
    {
      jm_.registProcMessageCb([this](JobMessage &msg)
                              { this->procAllMessage(msg); });
    }

    void procAllMessage(JobMessage &msg)
    {
      std::ostringstream oss;
      oss << "procAllMessage: event=" << msg.event << ", payload=\"" << msg.payload << "\"";
      log(oss.str());
    }

  private:
    static void log(const std::string &s)
    {
      std::cout << "[" << now() << "] [tid " << tid() << "] " << s << "\n";
    }

    JobManager &jm_;
  };

} // namespace demo

int main()
{
  using namespace demo;

  std::cout << "=== job_schedule 设计意义 demo ===\n";
  std::cout << "主线程 tid:    " << tid() << "\n";

  TaskManager taskManager;
  std::cout << "事件循环 tid:  " << taskManager.workerThreadId() << "\n\n";

  JobManager jobManager;
  JobExecutorManagerBase executor(jobManager);

  // 对应 middleware_jobs.cpp 里的绑定：
  // - dispatch: postFunc -> 排队到事件循环线程执行
  // - direct:   callFunc -> 立即在当前线程执行
  jobManager.registDispatchMsgCb([&](std::function<void()> &&func)
                                 {
    std::cout << "[" << now() << "] [tid " << tid() << "] dispatch: post 到事件循环\n";
    taskManager.postFunc(std::move(func)); });

  jobManager.registDirectCallMsgCb([&](std::function<void()> &&func)
                                   {
    std::cout << "[" << now() << "] [tid " << tid() << "] direct:  立即执行(不排队)\n";
    taskManager.callFunc(std::move(func)); });

  std::cout << "\n--- 场景 A：callMessage()（直调）---\n";
  JobMessage m1{.event = 1, .payload = "callMessage: 我在哪个线程被处理？"};
  jobManager.callMessage(m1);

  std::cout << "\n--- 场景 B：sendMessage()（投递）---\n";
  JobMessage m2{.event = 2, .payload = "sendMessage: 我在哪个线程被处理？"};
  jobManager.sendMessage(m2);

  std::cout << "\n--- 场景 C：连续 sendMessage()（验证顺序/串行）---\n";
  for (int i = 0; i < 5; ++i)
  {
    JobMessage mi{.event = 100 + i, .payload = "queued #" + std::to_string(i)};
    jobManager.sendMessage(mi);
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  std::cout << "\n结论提示：\n"
            << "- callMessage(): 处理发生在调用者线程（这里就是主线程 tid=" << tid() << "）\n"
            << "- sendMessage(): 处理发生在事件循环线程（tid=" << taskManager.workerThreadId() << "）\n"
            << "- 这就是为什么要把 procMessage(msg) 封装成 void() 任务再交给 dispatch：为了切线程/排队/解耦。\n";

  return 0;
}
