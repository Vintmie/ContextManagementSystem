#pragma once

#include <memory>
#include <vector>
#include <string>
#include "Scenario/ScenarioManager.h"
#include "FSManager.h"
#include "Utils.h"
#include <thread>
#include <atomic>

class UserInterface
{
public:
    UserInterface();
    void showMainMenu();

private:
    void createScenario();
    void viewScenarios();
    void executeScenario();
    void startPeriodicExecution();
    void stopPeriodicExecution();  // New method to stop the periodic execution
    void exitProgram();
    void displayConditionals() const;
    void displayTasks() const;

    void saveScenarioToFile(const std::shared_ptr<ScenarioManager>& scenario);
    void loadScenarioFromFile();
    void viewLoadedScenarios();
    ExecutionTypeCondition selectExecutionTypeCondition() const;

    std::unique_ptr<ScenarioManager> scenarioManager;
    std::unique_ptr<FSManager> fsManager;
    std::vector<std::shared_ptr<ScenarioManager>> scenarioUserBuffer;
    std::vector<std::shared_ptr<ScenarioManager>> scenarioFileBuffer;

    std::thread periodicExecutionThread;          // Thread for periodic execution
    std::atomic<bool> stopPeriodicExecutionFlag;  // Flag to stop the periodic execution
};
