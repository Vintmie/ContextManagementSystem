#include "Scenario/ScenarioStep.h"
#include <thread>
#include <chrono>

ScenarioStep::ScenarioStep(std::unique_ptr<IConditional> cond, std::unique_ptr<ITask> tsk, ExecutionTypeCondition cnd)
    : condition(std::move(cond)), task(std::move(tsk)), executionCondition(cnd)
{
}

ResultType ScenarioStep::executeTask(bool isLog)
{
    if (task)
    {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return task->execute(isLog);
    }
    return ResultType::UNKNOWN;
}

bool ScenarioStep::evaluateCondition(bool isLog) const
{
    if (!condition)
    {
        return false;
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return condition->evaluate(isLog);
}

ExecutionTypeCondition ScenarioStep::getExecutionCondition() const
{
    return executionCondition;
}

void ScenarioStep::setExecutionCondition(ExecutionTypeCondition cond)
{
    executionCondition = cond;
}

ResultType ScenarioStep::getExecutionResult() const
{
    return task->getExecutionResult();
}

const IConditional* ScenarioStep::getCondition() const
{
    return condition.get();
}

const ITask* ScenarioStep::getTask() const
{
    return task.get();
}