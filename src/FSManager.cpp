#include "FSManager.h"
#include "Scenario/ScenarioManager.h"
#include "Conditional/TimeCondition.h"
#include "Conditional/BatteryLevelCondition.h"
#include "Task/MessageBoxTask.h"
#include "Task/ChangePowerPlanTask.h"
#include <fstream>

void FSManager::saveScenarios(const ScenarioManager& manager) const
{
    nlohmann::json j;
    for (const auto& scenario : manager.getScenarios())
    {
        j.push_back(scenarioToJson(*scenario));
    }
    std::ofstream outFile(filePath);
    outFile << j.dump(4);
}

void FSManager::saveScenarios(const ScenarioManager& manager, const std::string& path) const
{
    nlohmann::json j;
    for (const auto& scenario : manager.getScenarios())
    {
        j.push_back(scenarioToJson(*scenario));
    }
    std::ofstream outFile(path);
    outFile << j.dump(4);
}

void FSManager::loadScenarios(ScenarioManager& manager) const
{
    std::ifstream inFile(filePath);
    if (!inFile) return;

    nlohmann::json j;
    inFile >> j;

    for (const auto& scenarioJson : j)
    {
        manager.addScenario(jsonToScenario(scenarioJson));
    }
}

void FSManager::loadScenarios(ScenarioManager& manager, const std::string& path) const
{
    std::ifstream inFile(path);
    if (!inFile) return;

    nlohmann::json j;
    inFile >> j;

    for (const auto& scenarioJson : j)
    {
        manager.addScenario(jsonToScenario(scenarioJson));
    }
}

nlohmann::json FSManager::scenarioToJson(const Scenario& scenario) const
{
    nlohmann::json j;
    for (const auto& step : scenario.getSteps())
    {
        j["steps"].push_back(stepToJson(*step));
    }
    return j;
}

std::shared_ptr<Scenario> FSManager::jsonToScenario(const nlohmann::json& j) const
{
    auto scenario = std::make_shared<Scenario>();
    for (const auto& stepJson : j["steps"])
    {
        scenario->addStep(jsonToStep(stepJson));
    }
    return scenario;
}

nlohmann::json FSManager::stepToJson(const ScenarioStep& step) const
{
    nlohmann::json j;
    j["condition"] = conditionToJson(*step.getCondition());
    j["task"] = taskToJson(*step.getTask());
    j["executionCondition"] = static_cast<int>(step.getExecutionCondition());
    return j;
}

std::shared_ptr<ScenarioStep> FSManager::jsonToStep(const nlohmann::json& j) const
{
    auto condition = jsonToCondition(j["condition"]);
    auto task = jsonToTask(j["task"]);
    auto executionCondition = static_cast<ExecutionTypeCondition>(j["executionCondition"].get<int>());
    return std::make_shared<ScenarioStep>(std::move(condition), std::move(task), executionCondition);
}

nlohmann::json FSManager::conditionToJson(const IConditional& condition) const
{
    nlohmann::json j;
    if (const auto* timeCond = dynamic_cast<const TimeCondition*>(&condition))
    {
        j["type"] = "TimeCondition";
        j["hour"] = timeCond->getHour();
    }
    else if (const auto* batteryCond = dynamic_cast<const BatteryLevelCondition*>(&condition))
    {
        j["type"] = "BatteryLevelCondition";
        j["crucialLevel"] = batteryCond->getCrucialLevel();
    }
    return j;
}

std::unique_ptr<IConditional> FSManager::jsonToCondition(const nlohmann::json& j) const
{
    std::string type = j["type"];
    if (type == "TimeCondition")
    {
        return std::make_unique<TimeCondition>(j["hour"].get<int>());
    }
    else if (type == "BatteryLevelCondition")
    {
        return std::make_unique<BatteryLevelCondition>(j["crucialLevel"].get<int>());
    }
    return nullptr;
}

nlohmann::json FSManager::taskToJson(const ITask& task) const
{
    nlohmann::json j;
    if (const auto* msgBoxTask = dynamic_cast<const MessageBoxTask*>(&task))
    {
        j["type"] = "MessageBoxTask";
    }
    else if (const auto* changePowerPlanTask = dynamic_cast<const ChangePowerPlanTask*>(&task))
    {
        j["type"] = "ChangePowerPlanTask";
    }
    return j;
}

std::unique_ptr<ITask> FSManager::jsonToTask(const nlohmann::json& j) const
{
    std::string type = j["type"];
    if (type == "MessageBoxTask")
    {
        return std::make_unique<MessageBoxTask>();
    }
    else if (type == "ChangePowerPlanTask")
    {
        return std::make_unique<ChangePowerPlanTask>();
    }
    return nullptr;
}
