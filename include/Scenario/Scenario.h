#pragma once
#include <vector>
#include <memory>
#include "ScenarioStep.h"

class Scenario {
public:
    void addStep(std::shared_ptr<ScenarioStep> step);

    bool execute();

    const std::vector<std::shared_ptr<ScenarioStep>>& getSteps() const { return steps; }

private:
    std::vector<std::shared_ptr<ScenarioStep>> steps;
};
