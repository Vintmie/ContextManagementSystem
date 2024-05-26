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
    // auto scenarioManager = std::make_unique<ScenarioManager>();
    std::string filePath = "scenarios.json";

    //////// Створення сценаріїв
    // auto scenario1 = std::make_shared<Scenario>();

    // auto step1 = std::make_shared<ScenarioStep>(
    //     std::make_unique<BatteryLevelCondition>(40), // USER SETS CURCIAL, CURRENT == 50.
    //     std::make_unique<ChangePowerPlanTask>(),
    //     ExecutionTypeCondition::FAILURE // DONT MATTER FOR FIRST WE OVERWRITE IT XAXA
    //);
    // scenario1->addStep(step1);

    // auto step2 = std::make_shared<ScenarioStep>(
    //     std::make_unique<TimeCondition>(19), // if not CURRENT TIME HOURS NUMBER yet perform box
    //     std::make_unique<MessageBoxTask>(),
    //     ExecutionTypeCondition::FAILURE
    //);
    // scenario1->addStep(step2);

    // scenarioManager->addScenario(scenario1);

    //////// Виконання сценаріїв перед збереженням
    // scenarioManager->executeScenarios();

    //// Збереження сценаріїв
    FSManager fsManager(filePath);
    // fsManager.saveScenarios(*scenarioManager);

    ////// Створення нового менеджера для завантаження сценаріїв
    auto loadedScenarioManager = std::make_unique<ScenarioManager>();
    fsManager.loadScenarios(*loadedScenarioManager);

    ////// Виконання завантажених сценаріїв
    loadedScenarioManager->executeScenarios();

    return 0;
}
