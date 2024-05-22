#pragma once
#include "ScenarioStep.h"
#include <iostream>

class ScenarioStepStart : public ScenarioStep {
public:
    ScenarioStepStart() : ScenarioStep(nullptr, nullptr) {}
    void executeTask() override {
        std::cout << "Scenario Started" << std::endl;
    }
};