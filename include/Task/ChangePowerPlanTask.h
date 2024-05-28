#pragma once
#include "ITask.h"
#include <iostream>
#include "spdlog/spdlog.h"

class ChangePowerPlanTask : public ITask
{
public:
    ChangePowerPlanTask() {}
    ResultType execute(bool isLog = true) override;

    ResultType getExecutionResult() const override;

private:
    ResultType changePowerPlan(bool isLog = true);

    ResultType currentExecutionResult = ResultType::UNKNOWN;
};