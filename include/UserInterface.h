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
    size_t displayConditionals() const;
    size_t displayTasks() const;


    std::vector<std::shared_ptr<Scenario>> getUnique(bool isNotThread = true);

    void loadScenariosFromDirectory();

    void showRunningScenarios();

    void startPeriodicExecution();
    void stopPeriodicExecution();
    void exitProgram();
    void stopSelectedScenario();
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
    std::vector<std::thread> periodicExecutionThreads;
    std::atomic<bool> stopPeriodicExecutionFlag;

    std::set<int> runningScenarioIds;
    static const int MAX_THREADS;
    const std::wstring& directoryPath = L"..\\bin\\start\\";
};
