#pragma once

#include <memory>
#include <vector>
#include <string>
#include "Scenario/ScenarioManager.h"

class UserInterface
{
public:
    UserInterface();

    void showMainMenu();
    void createScenario();
    void viewScenarios();
    void executeScenario();
    void exitProgram();

private:
    void displayConditionals() const;
    void displayTasks() const;
    ExecutionTypeCondition selectExecutionTypeCondition() const;

    std::unique_ptr<ScenarioManager> scenarioManager;
    std::vector<std::shared_ptr<Scenario>> scenarioBuffer;
    const std::string filePath = "D:\\UniversityKeep\\ContextManagementSystem\\bin\\tmp\\scenarios.json";
};
