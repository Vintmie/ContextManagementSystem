#include "Scenario/ScenarioManager.h"
#include "FormatOutput.h"
#include <iostream>

ScenarioManager::ScenarioManager() {}

ScenarioManager::~ScenarioManager() {}

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

        auto file_logger = LoggerManager::getThreadFileLogger(isLog);
        if (isLog != false)
        {
            spdlog::info("======= Scenario {} start\n", scenario->getName());
        }

        file_logger->info("======= Scenario {} start\n", scenario->getName(), thread_id_str);
        scenario->execute(isLog);
        if (isLog != false)
        {
            spdlog::info("Scenario {} end =======\n", scenario->getName());
        }
        file_logger->info("Scenario {} end =======\n", scenario->getName(), thread_id_str);
    }
}

void ScenarioManager::clearScenarios()
{
    scenarios.clear();
}