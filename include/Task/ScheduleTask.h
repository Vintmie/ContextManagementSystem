#pragma once
#include "ITask.h"
#include <iostream>
#include "spdlog/spdlog.h"

class ScheduleTask : public ITask
{
public:
    ScheduleTask() {}
    ScheduleTask(int time, std::wstring path) : startUpTime(time), startPath(path) {}
    ResultType execute(bool isLog = true) override;
    ResultType getExecutionResult() const override;

private:
    int startUpTime = 60;
    std::wstring startPath = L"C:\\Windows\\system32\\mspaint.exe";
    ResultType scheduleTask(std::string futureTime, bool isLog = true);
    ResultType currentExecutionResult = ResultType::UNKNOWN;
};