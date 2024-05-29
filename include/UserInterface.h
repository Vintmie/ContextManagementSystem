#pragma once

#include <memory>
#include <vector>
#include <string>
#include "Scenario/ScenarioManager.h"
#include "FSManager.h"
#include "Utils.h"
#include <thread>
#include <atomic>
#include <condition_variable>


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
    void stopPeriodicExecution(); // New method to stop the periodic execution
    void exitProgram();
    void displayConditionals() const;
    void displayTasks() const;

    void showRunningScenarios();

    void stopSelectedScenario();  // Додаємо оголошення нового методу

    void saveScenarioToFile(const std::shared_ptr<ScenarioManager>& scenario);
    void loadScenarioFromFile();
    void viewLoadedScenarios();
    ExecutionTypeCondition selectExecutionTypeCondition() const;

    std::unique_ptr<ScenarioManager> scenarioManager;
    std::unique_ptr<ScenarioManager> scenarioPeriodicManager;
    std::unique_ptr<FSManager> fsManager;
    std::vector<std::shared_ptr<ScenarioManager>> scenarioUserBuffer;
    std::vector<std::shared_ptr<ScenarioManager>> scenarioFileBuffer;
    


    std::condition_variable cv;
    std::mutex cv_m;
    std::thread periodicExecutionThread; // Thread for periodic execution
    std::atomic<bool> stopPeriodicExecutionFlag; // Flag to stop the periodic execution
};
