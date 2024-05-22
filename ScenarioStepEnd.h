#pragma once
#include "ScenarioStep.h"
#include <iostream>

class ScenarioStepEnd : public ScenarioStep {
public:
    ScenarioStepEnd() : ScenarioStep(nullptr, nullptr) {}
    void executeTask() override {
        std::cout << "Scenario Ended" << std::endl;
    }
};
