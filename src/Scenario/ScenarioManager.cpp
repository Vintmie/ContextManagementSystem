#include "Scenario/ScenarioManager.h"
#include "FormatOutput.h"
#include <iostream>

ScenarioManager::ScenarioManager()
{
    // spdlog::info("ScenarioManager start!");
    // std::cout << "------------------------------------------------------------\n";
}

ScenarioManager::~ScenarioManager()
{
    // std::cout << "------------------------------------------------------------\n";
    // spdlog::info("ScenarioManager end!");
}

void ScenarioManager::addScenario(std::shared_ptr<Scenario> scenario)
{
    scenarios.push_back(scenario);
}

void ScenarioManager::removeScenario(std::shared_ptr<Scenario> scenario)
{
    scenarios.erase(std::remove(scenarios.begin(), scenarios.end(), scenario), scenarios.end());

}

void ScenarioManager::executeScenarios(bool isLog)
{
    auto res_logger = LoggerManager::get_unique_logger();
    for (const auto& scenario : scenarios)
    {

        auto file_logger = LoggerManager::getFileLogger(isLog);
        if (isLog != false)
        {
            spdlog::info("======= Scenario {} start\n", scenario->getName());
        }

        file_logger->info("======= Scenario {} start\n", scenario->getName());
        scenario->execute(isLog);
        if (isLog != false)
        {
            spdlog::info("Scenario {} end =======\n", scenario->getName());
        }
        file_logger->info("Scenario {} end =======\n", scenario->getName());
    }
}

void ScenarioManager::clearScenarios()
{
    scenarios.clear();
}