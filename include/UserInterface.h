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
#include <set> 

class UserInterface
{
public:
    UserInterface();
    void showMainMenu();

private:
    void createScenario();
    void viewScenarios();
    void executeScenario();
    void displayConditionals() const;
    void displayTasks() const;

    void showRunningScenarios();


    void startPeriodicExecution();
    void stopPeriodicExecution();  // New method to stop the periodic execution
    void exitProgram();
    void stopSelectedScenario();  // Додаємо оголошення нового методу
    void stopAllThreads();


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
    std::vector<std::thread> periodicExecutionThreads;  // Thread for periodic execution
    std::atomic<bool> stopPeriodicExecutionFlag; // Flag to stop the periodic execution

    std::set<int> runningScenarioIds;  // Declare the set to store running scenario IDs
    static const int MAX_THREADS;
};
