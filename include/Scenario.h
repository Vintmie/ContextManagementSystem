#pragma once
#include <vector>
#include <memory>
#include "ScenarioStep.h"

class Scenario {
public:
    void addStep(std::shared_ptr<ScenarioStep> step) {
        steps.push_back(step);
    }

    bool execute() {
        ResultType prevResult = ResultType::SUCCESS;

        for (const auto& step : steps) {


                if (step->getExecutionCondition() != ExecutionTypeCondition::UNCONDITIONAL) {
                    // CHECK PREVIOUS STATE
                    if  (prevResult == ResultType::UNKNOWN ||
                        (prevResult == ResultType::SUCCESS && step->getExecutionCondition() == ExecutionTypeCondition::FAILURE) ||
                        (prevResult == ResultType::FAILURE && step->getExecutionCondition() == ExecutionTypeCondition::SUCCESS)) {
                        break;
                        return false; // END SCENARIO
                    }
                }
                if (step->evaluateCondition()) {
                    step->executeTask();
                    prevResult = step->getExecutionResult();
                }
                else prevResult = ResultType::UNKNOWN;
                continue;
        }
        return true;
    }

private:
    std::vector<std::shared_ptr<ScenarioStep>> steps;
};
