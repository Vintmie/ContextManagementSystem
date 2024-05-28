#pragma once
#include <memory>
#include "Conditional/IConditional.h"
#include "Task/ITask.h"

enum class ExecutionTypeCondition
{
    SUCCESS,
    FAILURE,
    UNCONDITIONAL
};

class ScenarioStep
{
public:
    ScenarioStep(
        std::unique_ptr<IConditional> cond, std::unique_ptr<ITask> tsk, ExecutionTypeCondition cnd = ExecutionTypeCondition::UNCONDITIONAL);

    virtual ResultType executeTask(bool isLog = true);

    virtual bool evaluateCondition(bool isLog = true) const;

    ExecutionTypeCondition getExecutionCondition() const;

    void setExecutionCondition(ExecutionTypeCondition cond);

    ResultType getExecutionResult() const;

    const IConditional* getCondition() const;

    const ITask* getTask() const;

private:
    std::unique_ptr<IConditional> condition;
    std::unique_ptr<ITask> task;
    ExecutionTypeCondition executionCondition = ExecutionTypeCondition::UNCONDITIONAL;
};
