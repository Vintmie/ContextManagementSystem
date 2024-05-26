#pragma once
#include <vector>
#include <memory>
#include "spdlog/spdlog.h"
#include "Scenario.h"

class ScenarioManager {
public:
    ScenarioManager();

    ~ScenarioManager();
    void addScenario(std::shared_ptr<Scenario> scenario);

    void executeScenarios();

    const std::vector<std::shared_ptr<Scenario>>& getScenarios() const { return scenarios; }


private:
    std::vector<std::shared_ptr<Scenario>> scenarios;
};
