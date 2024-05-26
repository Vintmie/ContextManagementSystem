#include "Task/ChangePowerPlanTask.h"

ResultType ChangePowerPlanTask::execute() {
        return changePowerPlan();
    }

ResultType ChangePowerPlanTask::getExecutionResult() const {
        return currentExecutionResult;
    }

ResultType ChangePowerPlanTask::changePowerPlan() {
        spdlog::info("Power plan changed due to low battery level\n");
        currentExecutionResult = ResultType::FAILURE;
        if(currentExecutionResult == ResultType::FAILURE) spdlog::info("changePowerPlan returns: FAILURE\n");
        else spdlog::info("changePowerPlan returns: SUCCESS\n");
        return currentExecutionResult;

}