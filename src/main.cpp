#include "Scenario/ScenarioManager.h"
#include "Conditional/TimeCondition.h"
#include "Conditional/BatteryLevelCondition.h"
#include "Task/MessageBoxTask.h"
#include "Task/ChangePowerPlanTask.h"
#include "FSManager.h"
#include <memory>
#include <iostream>

int main()
{
    auto scenarioManager = std::make_unique<ScenarioManager>();
    std::string filePath = "D:\\UniversityKeep\\ContextManagementSystem\\bin\\scenarios.json";

    // Створення сценаріїв
    auto scenario1 = std::make_shared<Scenario>();

    auto step1 = std::make_shared<ScenarioStep>(
        std::make_unique<BatteryLevelCondition>(60),
        std::make_unique<ChangePowerPlanTask>());
    scenario1->addStep(step1);

    auto step2 = std::make_shared<ScenarioStep>(
        std::make_unique<TimeCondition>(14),
        std::make_unique<MessageBoxTask>(), ExecutionTypeCondition::SUCCESS);
    scenario1->addStep(step2);

    scenarioManager->addScenario(scenario1);

    ////// Виконання сценаріїв перед збереженням
    scenarioManager->executeScenarios();

    //// Збереження сценаріїв
    // FSManager fsManager(filePath);
    // fsManager.saveScenarios(*scenarioManager);

    //// //// Створення нового менеджера для завантаження сценаріїв
    // auto loadedScenarioManager = std::make_unique<ScenarioManager>();
    // fsManager.loadScenarios(*loadedScenarioManager,
    // "D:\\UniversityKeep\\ContextManagementSystem\\bin\\liniar.json");

    //// ////// Виконання завантажених сценаріїв
    // loadedScenarioManager->executeScenarios();

    return 0;
}
