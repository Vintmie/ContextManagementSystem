#include "Scenario/ScenarioManager.h"

    ScenarioManager::ScenarioManager() {
        spdlog::info("ScenarioManager start!\n");
    }

     ScenarioManager::~ScenarioManager() {
        spdlog::info("ScenarioManager end!\n");
    }

    void ScenarioManager::addScenario(std::shared_ptr<Scenario> scenario) {
        scenarios.push_back(scenario);
    }

    void ScenarioManager::executeScenarios() {
        for (const auto& scenario : scenarios) {
            scenario->execute();
        }
    }