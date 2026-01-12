#ifndef JOBEXECUTOR_INTERFACE_HPP
#define JOBEXECUTOR_INTERFACE_HPP

#include <string>
#include <functional>

enum JobExecutorReturnCode
{
    JOB_EXECUTOR_RETURN_CODE_SUCCESS = 0,
    JOB_EXECUTOR_RETURN_CODE_ERROR = 1,
};

JobExecutorReturnCode JobExecutorRun();
void JobExecutorAddJob(const std::string &jobName,               // 任务名字
                       const std::function<void()> &jobFunction, // 任务函数
                       int interval,                             // 执行间隔
                       const std::string &params                 // 任务参数
);
#endif