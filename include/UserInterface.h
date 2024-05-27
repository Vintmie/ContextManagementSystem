#pragma once

#include <memory>
#include <vector>
#include <string>
#include "Scenario/ScenarioManager.h"
#include "FSManager.h"

class UserInterface
{
public:
    UserInterface();
    void showMainMenu();

private:
    void createScenario();
    void viewScenarios();
    void executeScenario();
    void exitProgram();
    void displayConditionals() const;
    void displayTasks() const;

    void loadScenarioFromFile(const std::string& filePath);  // Доданий метод для завантаження сценаріїв з файлу
    void viewLoadedScenarios();
    ExecutionTypeCondition selectExecutionTypeCondition() const;

    std::unique_ptr<ScenarioManager> scenarioManager;
    std::vector<std::shared_ptr<Scenario>> scenarioUserBuffer;
    std::vector<std::shared_ptr<Scenario>> scenarioFileBuffer;
    std::unique_ptr<FSManager> fsManager;

};