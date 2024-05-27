#include "UserInterface.h"
#include "Scenario/ScenarioManager.h"
#include "Scenario/ScenarioStep.h"
#include "Conditional/TimeCondition.h"
#include "Conditional/BatteryLevelCondition.h"
#include "Task/MessageBoxTask.h"
#include "Task/ChangePowerPlanTask.h"
#include "FSManager.h"

int main()
{
    UserInterface ui;
    ui.showMainMenu();

    ////Scenario scenario1;
    ////auto step1 = std::make_shared<ScenarioStep>(std::make_unique<BatteryLevelCondition>(40),  // user sets curcial, current == 50.
    ////    std::make_unique<ChangePowerPlanTask>()
    ////);
    //// scenario1.addStep(step1);

    //// std::string filePath = FSManager::generateUniqueFileName();
    //// //std::cout << filePath;

    // FSManager fsManager;
    // auto scenario3 = std::make_shared<Scenario>();
    // fsManager.loadScenario(scenario3, "D:\\UniversityKeep\\ContextManagementSystem\\.DB\\scenario_0.json");
    // //fsManager.saveScenario(scenario1, "D:\\UniversityKeep\\ContextManagementSystem\\.DB\\scenario_1.json");
    // //auto loadedScenarioManager = std::make_unique<ScenarioManager>();
    // 
    // //std::shared_ptr<Scenario> scenario = std::make_shared<Scenario>();
    // //fsManager.loadScenario(scenario, "D:\\UniversityKeep\\ContextManagementSystem\\.DB\\scenario_0.json");
    // scenario3->execute();
 
 
    return 0;
}
