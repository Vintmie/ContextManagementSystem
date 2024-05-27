//#include "UserInterface.h"
//#include "Scenario/ScenarioManager.h"
//#include "Conditional/TimeCondition.h"
//#include "Conditional/BatteryLevelCondition.h"
//#include "Task/MessageBoxTask.h"
//#include "Task/ChangePowerPlanTask.h"
//
//int main()
//{
//    /*   UserInterface ui;
//       ui.showMainMenu();*/
//
//    auto scenario1 = std::make_shared<Scenario>("Helloo", "Neeed more");
//
//    auto step1 = std::make_shared<ScenarioStep>(std::make_unique<BatteryLevelCondition>(60),  // USER SETS CURCIAL, CURRENT == 50.
//        std::make_unique<ChangePowerPlanTask>());
//    scenario1->addStep(step1);
//
//    auto step2 = std::make_shared<ScenarioStep>(std::make_unique<TimeCondition>(22),  // if not CURRENT TIME HOURS NUMBER yet perform box
//        std::make_unique<MessageBoxTask>(), ExecutionTypeCondition::SUCCESS);
//    scenario1->addStep(step2);
//
//
//
//    scenario1->execute();
//
//    return 0;
//}

#include <iostream>
#include "FSManager.h"
#include "Scenario/Scenario.h"
#include "Scenario/ScenarioManager.h"
#include "Conditional/TimeCondition.h"
#include "Conditional/BatteryLevelCondition.h"
#include "Task/MessageBoxTask.h"
#include "Task/ChangePowerPlanTask.h"

int main()
{
    // Створення об'єкту FSManager
    FSManager fsManager;

    // Створення сценарію
    //std::string scenarioName = "SampleScenario";
    //std::string scenarioDescription = "This is a sample scenario.";

    auto scenarioManager = std::make_unique<ScenarioManager>();

    //auto scenario1 = std::make_shared<Scenario>(scenarioName, scenarioDescription);
    //auto scenario2 = std::make_shared<Scenario>();
    //auto scenario3 = std::make_shared<Scenario>();

    //auto step1 = std::make_shared<ScenarioStep>(std::make_unique<BatteryLevelCondition>(60),  // USER SETS CURCIAL, CURRENT == 50.
    //    std::make_unique<ChangePowerPlanTask>());
    //scenario1->addStep(step1);

    //auto step2 = std::make_shared<ScenarioStep>(std::make_unique<TimeCondition>(17),  // if not CURRENT TIME HOURS NUMBER yet perform box
    //    std::make_unique<MessageBoxTask>(), ExecutionTypeCondition::FAILURE);
    //scenario2->addStep(step2);
    ////scenarioManager->addScenario(scenario1);
    ////scenarioManager->addScenario(scenario2);
    //scenarioManager->addScenario(scenario3);
    //scenarioManager->executeScenarios();

    //fsManager.saveScenarios(*scenarioManager, "D:\\UniversityKeep\\ContextManagementSystem\\.DB\\blink.json");

    //auto scenarioManagerEX = std::make_unique<ScenarioManager>();
    auto scenarioManagerEX = std::make_unique<Scenario>();
    fsManager.loadScenarios(*scenarioManagerEX, "D:\\UniversityKeep\\ContextManagementSystem\\.DB\\blink.json");

    scenarioManagerEX->execute();
    //auto scenario3 = std::make_shared<Scenario>();
    //fsManager.loadScenarioDetails(*scenario3, "D:\\UniversityKeep\\ContextManagementSystem\\.DB\\TEST-TEst-new.json");
    //scenario3->execute();
    //std::cout << scenario3->getDescription();

    //std::cout << std::endl;
    //std::cout << scenario3->getName();
    //std::cout << std::endl;
    //scenarioManager->addScenario(scenario3);

    //fsManager.saveScenarioDetails(*scenario3, "D:\\UniversityKeep\\ContextManagementSystem\\.DB\\TEST-TEst-new.json");
    //fsManager.saveScenario(*scenario1, "D:\\UniversityKeep\\ContextManagementSystem\\.DB\\TEST-old.json");
 

    return 0;
}
