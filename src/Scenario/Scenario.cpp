#include "Scenario/ScenarioStep.h"
#include "Scenario/Scenario.h"

    void Scenario::addStep(std::shared_ptr<ScenarioStep> step) {
        if (steps.size() == 0) step->setExecutionCondition(ExecutionTypeCondition::UNCONDITIONAL);
        steps.push_back(step);
    }

bool Scenario::execute() {
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