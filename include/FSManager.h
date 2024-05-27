#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Scenario/ScenarioManager.h"
#include "nlohmann/json.hpp"

class FSManager
{
public:
    FSManager(const std::string& filePath) : filePath(filePath) {}

    void saveScenarios(const ScenarioManager& manager) const;
    void saveScenarios(const ScenarioManager& manager, const std::string& path) const;
    void loadScenarios(ScenarioManager& manager) const;
    void loadScenarios(ScenarioManager& manager, const std::string& path) const;

private:
    std::string filePath;

    nlohmann::json scenarioToJson(const Scenario& scenario) const;
    nlohmann::json stepToJson(const ScenarioStep& step) const;
    nlohmann::json conditionToJson(const IConditional& condition) const;
    nlohmann::json taskToJson(const ITask& task) const;

    std::shared_ptr<Scenario> jsonToScenario(const nlohmann::json& j) const;
    std::shared_ptr<ScenarioStep> jsonToStep(const nlohmann::json& j) const;
    std::unique_ptr<IConditional> jsonToCondition(const nlohmann::json& j) const;
    std::unique_ptr<ITask> jsonToTask(const nlohmann::json& j) const;
};
