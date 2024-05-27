#pragma once
#include "ITask.h"
#include <iostream>
#include "spdlog/spdlog.h"

class ScheduleTask : public ITask
{
public:
    ScheduleTask(){}
    ResultType execute() override;

    ResultType getExecutionResult() const override;

private:
    ResultType scheduleTask(std::string futureTime);
 
    ResultType currentExecutionResult = ResultType::UNKNOWN;
};