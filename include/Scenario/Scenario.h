#pragma once
#include <vector>
#include <memory>
#include <string>
#include "ScenarioStep.h"
#include <thread>
#include <atomic>
#include <condition_variable>

class Scenario
{
public:
    Scenario();
    Scenario(const std::string& name, const std::string& description);

    void addStep(std::shared_ptr<ScenarioStep> step);

    bool execute(bool isLog = true);

    const std::vector<std::shared_ptr<ScenarioStep>>& getSteps() const { return steps; }
    void setName(const std::string& name);
    std::string getName() const;

    void setDescription(const std::string& description);
    std::string getDescription() const;

    int getId() const { return id; }
    

private:
    std::string generateUniqueScenarioName() const;
    std::vector<std::shared_ptr<ScenarioStep>> steps;
    std::string name;
    std::string description;
    int id;  // Add a member to store the scenario ID
    static int nextId;
};
