#pragma once
#include "ITask.h"
#include <iostream>
#include "spdlog/spdlog.h"

class ChangePowerPlanTask : public ITask {
public:
    ChangePowerPlanTask( ) {}
    ResultType execute() override {
        return changePowerPlan();
    }

    ResultType getExecutionResult() const override {
        return currentExecutionResult;

    }

private:
    ResultType changePowerPlan() {
        spdlog::info("Power plan changed due to low battery level\n");
        currentExecutionResult = ResultType::FAILURE;
        if(currentExecutionResult == ResultType::FAILURE) spdlog::info("changePowerPlan returns: FAILURE\n");
        else spdlog::info("changePowerPlan returns: SUCCESS\n");
        return currentExecutionResult;

    }

    ResultType currentExecutionResult = ResultType::UNKNOWN;
};