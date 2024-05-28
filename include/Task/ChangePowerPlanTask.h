#pragma once
#include "ITask.h"
#include <iostream>
#include "spdlog/spdlog.h"

class ChangePowerPlanTask : public ITask
{
public:
    ChangePowerPlanTask() {}
    ResultType execute() override;

    ResultType getExecutionResult() const override;

private:
    ResultType changePowerPlan();

    ResultType currentExecutionResult = ResultType::UNKNOWN;
};