#pragma once
#include <vector>
#include <memory>
#include "spdlog/spdlog.h"
#include "Scenario.h"

class ScenarioManager {
public:
    ScenarioManager() {
        spdlog::info("ScenarioManager start!\n");
    }

    ~ ScenarioManager() {
        spdlog::info("ScenarioManager end!\n");
    }
    void addScenario(std::shared_ptr<Scenario> scenario) {
        scenarios.push_back(scenario);
    }

    void executeScenarios() {
        for (const auto& scenario : scenarios) {
            scenario->execute();
        }
    }

private:
    std::vector<std::shared_ptr<Scenario>> scenarios;
};
