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

void ScenarioManager::executeScenarios()
{
    auto res_logger = LoggerManager::get_unique_logger();
    for (const auto& scenario : scenarios)
    {

        auto file_logger = LoggerManager::getFileLogger();
        // res_logger->info("======= Scenario {} start\n", scenario->getName());
        spdlog::info("======= Scenario {} start\n", scenario->getName());
        file_logger->info("======= Scenario {} start\n", scenario->getName());
        scenario->execute();
        spdlog::info("Scenario {} end =======\n", scenario->getName());
        file_logger->info("Scenario {} end =======\n", scenario->getName());
    }
}

void ScenarioManager::clearScenarios()
{
    scenarios.clear();
}