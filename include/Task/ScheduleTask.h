#pragma once
#include "ITask.h"
#include <iostream>
#include "spdlog/spdlog.h"

class ScheduleTask : public ITask
{
public:
    ScheduleTask() {}
    ScheduleTask(int time, std::wstring path) : startUpTime(time), startPath(path) {}
    ResultType execute() override;
    ResultType getExecutionResult() const override;

private:
    int startUpTime = 60;
    std::wstring startPath = L"C:\\Windows\\system32\\mspaint.exe";
    ResultType scheduleTask(std::string futureTime);
    ResultType currentExecutionResult = ResultType::UNKNOWN;
};