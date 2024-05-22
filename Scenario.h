#pragma once
#include <vector>
#include <memory>
#include <iostream>
#include "ScenarioStep.h"
#include "ScenarioStepStart.h"
#include "ScenarioStepEnd.h"

class Scenario {
public:
    Scenario() {
        // A Scenario should always consist of at least 3 ScenarioSteps
        // 1) ScenarioStepStart
        // 2) ScenarioStep - adds the USER
        // 3) ScenarioStepEnd


        // Add the starting step
        steps.emplace_back(std::make_shared<ScenarioStepStart>());
        // Add the final step
        steps.emplace_back(std::make_shared<ScenarioStepEnd>());
    }

    void addStep(std::shared_ptr<ScenarioStep> step) { /* */ }

    bool execute() { /* */ }

private:
    std::vector<std::shared_ptr<ScenarioStep>> steps;
};
