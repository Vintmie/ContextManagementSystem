#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Scenario/ScenarioManager.h"
#include "nlohmann/json.hpp"
#include "Utils.h"

class FSManager
{
public:
    FSManager() {}
    FSManager(const std::string& filePath) : filePath(filePath) {}


    void saveScenarios(const ScenarioManager& manager) const;
    void saveScenarios(const ScenarioManager& manager, const std::string& path) const;

    void loadScenarios(ScenarioManager& manager) const;
    void loadScenarios(ScenarioManager& manager, const std::string& path) const;

    void saveScenario(const Scenario& scenario, const std::string& filePath) const;
    void loadScenario(std::shared_ptr<Scenario>& scenario, const std::string& filePath) const;

    void saveScenarioDialog(const Scenario& scenario) const;
    void loadScenarioDialog(std::shared_ptr<Scenario>& scenario) const;


    // NEW
    void saveScenarioDetails(const Scenario& scenario, const std::string& filePath) const;
    void loadScenarioDetails(Scenario& scenario, const std::string& filePath) const;
    nlohmann::json scenarioToJsonEXTENDED(const Scenario& scenario) const;
    std::shared_ptr<Scenario> jsonToScenarioEXTENDED(const nlohmann::json& j) const;


    static std::string generateUniqueFileName();

private:
    std::string filePath = "D:\\UniversityKeep\\ContextManagementSystem\\.DB\\default.json";
    static const std::string path;

    // nlohmann::json scenarioToJson(const Scenario& scenario) const;
    nlohmann::json stepToJson(const ScenarioStep& step) const;
    nlohmann::json conditionToJson(const IConditional& condition) const;
    nlohmann::json taskToJson(const ITask& task) const;

    //std::shared_ptr<Scenario> jsonToScenario(const nlohmann::json& j) const;
    std::shared_ptr<ScenarioStep> jsonToStep(const nlohmann::json& j) const;
    std::unique_ptr<IConditional> jsonToCondition(const nlohmann::json& j) const;
    std::unique_ptr<ITask> jsonToTask(const nlohmann::json& j) const;
};
