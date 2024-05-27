#include "UserInterface.h"
#include <iostream>
#include "FSManager.h"
#include "Scenario/Scenario.h"
#include "Scenario/ScenarioManager.h"
#include "Conditional/TimeCondition.h"
#include "Conditional/BatteryLevelCondition.h"
#include "Task/MessageBoxTask.h"
#include "Task/ChangePowerPlanTask.h"
#include "Task/ScheduleTask.h"

int main()
{
    /*
       UserInterface ui;
       ui.showMainMenu();
       */
     

   
    // Створення об'єкту FSManager
    FSManager fsManager;

    // Створення сценарію


    //auto scenarioManager = std::make_unique<ScenarioManager>();

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


    auto scenarioManagerEX = std::make_unique<ScenarioManager>();
     std::string scenarioName = "Sam";
     std::string scenarioDescription = "This is a hard and interestin";
     auto scenario1 = std::make_shared<Scenario>();
      auto step1 = std::make_shared<ScenarioStep>(std::make_unique<BatteryLevelCondition>(99),  // USER SETS CURCIAL, CURRENT == 50.
         std::make_unique<ChangePowerPlanTask>());

       auto step2 = std::make_shared<ScenarioStep>(std::make_unique<TimeCondition>(5),  // USER SETS CURCIAL, CURRENT == 50.
          std::make_unique<ScheduleTask>(),
           ExecutionTypeCondition::SUCCESS);

      scenario1->addStep(step1);
      scenario1->addStep(step2);
      scenarioManagerEX->addScenario(scenario1);
      scenarioManagerEX->executeScenarios();

       //fsManager.saveScenario(*scenario1, "D:\\UniversityKeep\\ContextManagementSystem\\.DB\\SAVESTRANGE.json");
     
    fsManager.saveScenarios(*scenarioManagerEX, "D:\\UniversityKeep\\ContextManagementSystem\\.DB\\Battery.json");
    //fsManager.saveScenarioDetails(*scenario1, "D:\\UniversityKeep\\ContextManagementSystem\\.DB\\scenarioDET.json");
   
    /*

    auto scenario2 = std::make_shared<Scenario>();

    auto scenario23 = std::make_shared<ScenarioManager>();


    //fsManager.loadScenario(scenario2, "D:\\UniversityKeep\\ContextManagementSystem\\.DB\\23.json");
    //fsManager.loadScenarioDialog(scenario2);
    //scenario2->execute();




    fsManager.loadScenarios(*scenario23, "D:\\UniversityKeep\\ContextManagementSystem\\.DB\\MUCH!.json");
    scenario23->executeScenarios();


    */
    /*scenario2->execute();
    std::cout << scenario2->getName();
    std::cout << std::endl;
    std::cout << scenario2->getDescription();
    std::cout << std::endl;*/
    //scenarioManagerEX->executeScenarios();
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
