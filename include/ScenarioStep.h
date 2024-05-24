#pragma once
#include <memory>
#include "Conditional/IConditional.h"
#include "Task/ITask.h"

enum class ExecutionTypeCondition {
        SUCCESS,
        FAILURE,
        UNCONDITIONAL
    };

class ScenarioStep {
public:
    ScenarioStep(std::unique_ptr<IConditional> cond, std::unique_ptr<ITask> tsk, ExecutionTypeCondition cnd = ExecutionTypeCondition::UNCONDITIONAL)
        : condition(std::move(cond)), task(std::move(tsk)), executionCondition(cnd) { }

    virtual ResultType executeTask() {
        if (task) {
            return task->execute();
        }
        return ResultType::UNKNOWN;
    }

    virtual bool evaluateCondition() const {
        if (!condition) {
            return false;
        }
        return condition->evaluate();
    }

    ExecutionTypeCondition getExecutionCondition() const {
        return executionCondition;
    }

    ResultType getExecutionResult() const {
        return task->getExecutionResult();
    }

private:
    std::unique_ptr<IConditional> condition;
    std::unique_ptr<ITask> task;
    ExecutionTypeCondition executionCondition = ExecutionTypeCondition::UNCONDITIONAL;
  
};
