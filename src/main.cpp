#include "ScenarioManager.h"
#include "Conditional/TimeCondition.h"
#include "Conditional/BatteryLevelCondition.h"
#include "Task/MessageBoxTask.h"
#include "Task/ChangePowerPlanTask.h"

#include <memory>

int main() {
    auto scenarioManager = std::make_unique<ScenarioManager>();

    auto scenario1 = std::make_shared<Scenario>();

    auto step1 = std::make_shared<ScenarioStep>(
           std::make_unique<BatteryLevelCondition>(60), // USER SETS CURCIAL, CURRENT == 50. 
           std::make_unique<ChangePowerPlanTask>()
    );
    scenario1->addStep(step1);

    auto step2 = std::make_shared<ScenarioStep>(
        std::make_unique<TimeCondition>(17),            // if not CURRENT TIME HOURS NUMBER yet perform box
        std::make_unique<MessageBoxTask>(),
        ExecutionTypeCondition::FAILURE
    );
    scenario1->addStep(step2);

    scenarioManager->addScenario(scenario1);

    scenarioManager->executeScenarios();

    return 0;
}