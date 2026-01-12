#include "jobexecutor.hpp"
#include <iostream>

static JobExecutor *JobExecutorInstance = nullptr;

JobExecutorReturnCode JobExecutorRun()
{
    return JobExecutorInstance->getInstance()->executeJob();
}

void JobExecutorAddJob(const std::string &jobName,
                       const std::function<void()> &jobFunction,
                       int interval,
                       const std::string &params)
{
    JobExecutorInstance->getInstance()->addJob(jobName, jobFunction, interval, params);
}

JobExecutor::JobExecutor()
{
}

JobExecutor::~JobExecutor()
{
    if (JobExecutorInstance != nullptr)
    {
        delete JobExecutorInstance;
        JobExecutorInstance = nullptr;
    }
}

void JobExecutor::addJob(const std::string &jobName,
                         const std::function<void()> &jobFunction,
                         int interval,
                         const std::string &params)
{
    std::cout << "addJob: " << jobName << std::endl;
    jobMap.emplace(jobName, JobInfo(jobName, jobFunction, interval, params));
    std::cout << "jobMap size: " << jobMap.size() << std::endl;
}
JobExecutor *JobExecutor::getInstance()
{
    if (JobExecutorInstance == nullptr)
    {
        JobExecutorInstance = new JobExecutor();
    }
    // 打印
    std::cout << "JobExecutorInstance: " << std::endl;
    return (JobExecutor *)JobExecutorInstance;
}

JobExecutorReturnCode JobExecutor::executeJob()
{
    auto now = std::chrono::steady_clock::now();

    for (auto &pair : jobMap)
    {
        JobInfo &job = pair.second;

        // 计算距离上次执行的时间间隔（毫秒）
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                           now - job.lastExecuteTime)
                           .count();

        // 如果时间间隔已到，执行任务
        if (elapsed >= job.interval)
        {
            try
            {
                std::cout << "[JobExecutor] 执行任务: " << job.name
                          << " (间隔: " << job.interval << "ms)" << std::endl;
                std::cout << "任务参数: " << job.params << std::endl;
                job.function();            // 执行任务函数
                job.lastExecuteTime = now; // 更新上次执行时间
            }
            catch (const std::exception &e)
            {
                std::cerr << "[JobExecutor] 任务执行失败: " << job.name
                          << ", 错误: " << e.what() << std::endl;
                return JOB_EXECUTOR_RETURN_CODE_ERROR;
            }
        }
    }
    return JOB_EXECUTOR_RETURN_CODE_SUCCESS;
}
