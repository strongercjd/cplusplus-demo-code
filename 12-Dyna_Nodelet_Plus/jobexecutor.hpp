#ifndef JOB_EXECUTOR_HPP
#define JOB_EXECUTOR_HPP

#include <map>
#include <string>
#include <functional>
#include <chrono>
#include "jobexecutor_interface.hpp"

// 任务信息结构体
struct JobInfo
{
    std::string name;               // 任务名字
    std::function<void()> function; // 任务函数（已绑定参数）
    int interval;                   // 执行间隔（毫秒）

    std::chrono::steady_clock::time_point lastExecuteTime; // 上次执行时间

    std::string params; // 参数存储

    JobInfo(const std::string &n, const std::function<void()> &f, int i, const std::string &p)
        : name(n), function(f), interval(i), params(p), lastExecuteTime(std::chrono::steady_clock::now()) {}
};

class JobExecutor
{
public:
    JobExecutor();
    ~JobExecutor();

    JobExecutorReturnCode executeJob();

    JobExecutor *getInstance();
    void addJob(const std::string &jobName,
                const std::function<void()> &jobFunction,
                int interval,
                const std::string &params);

private:
    std::map<std::string, JobInfo> jobMap;
};

#endif