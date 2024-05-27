#include "Task/ChangePowerPlanTask.h"
#include "FormatOutput.h"

ResultType ChangePowerPlanTask::execute()
{
    return changePowerPlan();
}

ResultType ChangePowerPlanTask::getExecutionResult() const
{
    return currentExecutionResult;
}

ResultType ChangePowerPlanTask::changePowerPlan()
{
    auto res_logger = LoggerManager::get_unique_logger();
    currentExecutionResult = ResultType::SUCCESS;
    res_logger->info("ChangePowerPlanTask returned: {}\n", currentExecutionResult);

    return currentExecutionResult;
}