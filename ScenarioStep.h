#pragma once
#include <memory>
#include "IConditional.h"
#include "ITask.h"

class ScenarioStep {
public:
    ScenarioStep(std::shared_ptr<IConditional> cond, std::shared_ptr<ITask> tsk)
        : condition(cond), task(tsk) {}

    virtual void executeTask() { /* */ }

    virtual bool evaluateCondition() { /* */ }

protected:
    std::shared_ptr<IConditional> condition;
    std::shared_ptr<ITask> task;
};
