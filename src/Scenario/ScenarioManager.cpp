#include "Scenario/ScenarioManager.h"
#include <iostream>

ScenarioManager::ScenarioManager()
{
    spdlog::info("ScenarioManager start!");
    std::cout << "------------------------------------------------------------\n";
}

ScenarioManager::~ScenarioManager()
{
    std::cout << "------------------------------------------------------------\n";
    spdlog::info("ScenarioManager end!");
}

void ScenarioManager::addScenario(std::shared_ptr<Scenario> scenario)
{
    scenarios.push_back(scenario);
}

void ScenarioManager::executeScenarios()
{
    for (const auto& scenario : scenarios)
    {
        spdlog::info("======= Scenario start!");
        scenario->execute();
        spdlog::info("Scenario end! ======= ");
    }
}

void ScenarioManager::clearScenarios()
{
    scenarios.clear();
}