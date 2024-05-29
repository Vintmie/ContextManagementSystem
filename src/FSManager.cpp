#include "FSManager.h"
#include "Scenario/ScenarioManager.h"
#include "Conditional/TimeCondition.h"
#include "Conditional/BatteryLevelCondition.h"
#include "Task/MessageBoxTask.h"
#include "Task/ChangePowerPlanTask.h"
#include "Task/ScheduleTask.h"
#include <fstream>

const std::string FSManager::path = "..\\bin\\";

void FSManager::saveScenarios(const ScenarioManager& manager) const
{
    nlohmann::json j;
    for (const auto& scenario : manager.getScenarios())
    {
        j.push_back(scenarioToJsonEXTENDED(*scenario));
    }
    std::ofstream outFile(filePath);
    outFile << j.dump(4);
}

void FSManager::saveScenarios(const ScenarioManager& manager, const std::string& path) const
{
    nlohmann::json j;
    for (const auto& scenario : manager.getScenarios())
    {
        j.push_back(scenarioToJsonEXTENDED(*scenario));
    }
    std::ofstream outFile(path);
    outFile << j.dump(4);
}

nlohmann::json FSManager::scenarioToJsonEXTENDED(const Scenario& scenario) const
{
    nlohmann::json j;

    j["name"] = scenario.getName();
    j["description"] = scenario.getDescription();

    for (const auto& step : scenario.getSteps())
    {
        j["steps"].push_back(stepToJson(*step));
    }

    return j;
}

void FSManager::saveScenario(const Scenario& scenario, const std::string& filePath) const
{
    nlohmann::json j;

    j = scenarioToJsonEXTENDED(scenario);

    std::ofstream outFile(filePath);
    if (!outFile.is_open())
    {
        std::cerr << "Помилка відкриття файлу для запису сценарію.\n";
        return;
    }

    outFile << j.dump(4);
    outFile.close();
}

void FSManager::loadScenarios(ScenarioManager& manager) const
{
    std::ifstream inFile(filePath);
    if (!inFile) return;

    nlohmann::json j;
    inFile >> j;

    for (const auto& scenarioJson : j)
    {
        manager.addScenario(jsonToScenarioEXTENDED(scenarioJson));
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
        manager.addScenario(jsonToScenarioEXTENDED(scenarioJson));
    }
}

std::shared_ptr<Scenario> FSManager::jsonToScenarioEXTENDED(const nlohmann::json& j) const
{
    auto scenario = std::make_shared<Scenario>();

    scenario->setName(j["name"].get<std::string>());
    scenario->setDescription(j["description"].get<std::string>());

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
    else if (const auto* scheduleTask = dynamic_cast<const ScheduleTask*>(&task))  // Add this block
    {
        j["type"] = "ScheduleTask";
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
    else if (type == "ScheduleTask")
    {
        return std::make_unique<ScheduleTask>();
    }
    return nullptr;
}

void FSManager::loadScenario(std::shared_ptr<Scenario>& scenario, const std::string& filePath) const
{
    std::ifstream inFile(filePath);
    if (!inFile.is_open())
    {
        std::cerr << "Помилка відкриття файлу для завантаження сценарію.\n";
        return;
    }

    nlohmann::json j;
    inFile >> j;

    scenario->setName(j["name"].get<std::string>());
    scenario->setDescription(j["description"].get<std::string>());

    for (const auto& stepJson : j["steps"])
    {
        scenario->addStep(jsonToStep(stepJson));
    }
    inFile.close();
}

std::string FSManager::generateUniqueFileName()
{
    static std::atomic<int> counter{0};
    std::ostringstream oss;
    oss << path << "scenario_" << counter++ << ".json";
    return oss.str();
}

void FSManager::saveScenarioDialog(const Scenario& scenario) const
{
    std::wstring filePath =
        Utils::SaveFileSelectionDialog(OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT, L"Вкажіть назву файлу сценарію", L"JSON Files\0*.json\0");

    nlohmann::json j;
    j["name"] = scenario.getName();
    j["description"] = scenario.getDescription();

    for (const auto& step : scenario.getSteps())
    {
        j["steps"].push_back(stepToJson(*step));
    }

    std::ofstream outFile(Utils::wstring_to_utf8(filePath));
    if (!outFile.is_open())
    {
        std::cerr << "Помилка відкриття файлу для запису сценарію.\n";
        return;
    }

    outFile << j.dump(4);
    outFile.close();
    std::cout << "Сценарій збережено у файл.\n";
}

void FSManager::loadScenarioDialog(std::shared_ptr<Scenario>& scenario) const
{
    std::wstring filePath = Utils::OpenFileSelectionDialog(
        OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, L"Виберіть файл сценарію", L"JSON Files\0*.json\0");
    std::ifstream inFile(Utils::wstring_to_utf8(filePath));
    if (!inFile.is_open())
    {
        std::cerr << "Помилка відкриття файлу для завантаження сценарію.\n";
        return;
    }

    nlohmann::json j;
    inFile >> j;

    scenario->setName(j["name"].get<std::string>());
    scenario->setDescription(j["description"].get<std::string>());

    auto scenario1 = std::make_shared<Scenario>();
    for (const auto& stepJson : j["steps"])
    {
        scenario1->addStep(jsonToStep(stepJson));
    }

    scenario = scenario1;
    inFile.close();
}

void FSManager::saveScenarioDetails(const Scenario& scenario, const std::string& filePath) const
{
    nlohmann::json j;
    j["name"] = scenario.getName();
    j["description"] = scenario.getDescription();
    for (const auto& step : scenario.getSteps())
    {
        j["steps"].push_back(stepToJson(*step));
    }

    std::ofstream outFile(filePath);
    if (!outFile.is_open())
    {
        std::cerr << "Помилка відкриття файлу для запису деталей сценарію.\n";
        return;
    }

    outFile << j.dump(4);
    outFile.close();
}

void FSManager::loadScenarioDetails(Scenario& scenario, const std::string& filePath) const
{
    std::ifstream inFile(filePath);
    if (!inFile.is_open())
    {
        std::cerr << "Помилка відкриття файлу для завантаження деталей сценарію.\n";
        return;
    }

    nlohmann::json j;
    inFile >> j;

    scenario.setName(j["name"]);
    scenario.setDescription(j["description"]);

    for (const auto& stepJson : j["scenario"]["steps"])
    {
        scenario.addStep(jsonToStep(stepJson));
    }

    inFile.close();
}
