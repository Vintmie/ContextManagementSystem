#include "Scenario/Scenario.h"
#include "FormatOutput.h"

void Scenario::addStep(std::shared_ptr<ScenarioStep> step)
{
    if (steps.size() == 0) step->setExecutionCondition(ExecutionTypeCondition::UNCONDITIONAL);
    steps.push_back(step);
}

bool Scenario::execute()
{
    ResultType prevResult = ResultType::SUCCESS;
    auto condLogger = LoggerManager::get_unique_logger();
    for (const auto& step : steps)
    {
        condLogger->info("Current condition: {}\n", step->getExecutionCondition());
        if (step->getExecutionCondition() != ExecutionTypeCondition::UNCONDITIONAL)
        {
            // CHECK PREVIOUS STATE
            if (prevResult == ResultType::UNKNOWN ||
                (prevResult == ResultType::SUCCESS && step->getExecutionCondition() == ExecutionTypeCondition::FAILURE) ||
                (prevResult == ResultType::FAILURE && step->getExecutionCondition() == ExecutionTypeCondition::SUCCESS))
            {
                break;
                return false;  // END SCENARIO
            }
        }
        if (step->evaluateCondition())
        {
            step->executeTask();
            prevResult = step->getExecutionResult();
        }
        else
            prevResult = ResultType::UNKNOWN;
        continue;
    }
    return true;
}