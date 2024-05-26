#include "Scenario/ScenarioStep.h"

    ScenarioStep::ScenarioStep(std::unique_ptr<IConditional> cond, std::unique_ptr<ITask> tsk, ExecutionTypeCondition cnd)
        : condition(std::move(cond)), task(std::move(tsk)), executionCondition(cnd) { }

    ResultType ScenarioStep::executeTask() {
        if (task) {
            return task->execute();
        }
        return ResultType::UNKNOWN;
    }

    bool ScenarioStep::evaluateCondition() const {
        if (!condition) {
            return false;
        }
        return condition->evaluate();
    }

    ExecutionTypeCondition ScenarioStep::getExecutionCondition() const {
        return executionCondition;
    }
    
    void ScenarioStep::setExecutionCondition(ExecutionTypeCondition cond) {
        executionCondition = cond;
    }

    ResultType ScenarioStep::getExecutionResult() const {
        return task->getExecutionResult();
    }

    const IConditional* ScenarioStep::getCondition() const {
        return condition.get();
    }

    const ITask* ScenarioStep::getTask() const {
        return task.get();
    }