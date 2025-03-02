#include "Scenario/Scenario.h"
#include "FormatOutput.h"

int Scenario::nextId = 0;

Scenario::Scenario() : name(generateUniqueScenarioName()), description("DefaultDescription"), id(nextId++) {}

Scenario::Scenario(const std::string& name, const std::string& description) : name(name), description(description), id(nextId++) {}

void Scenario::addStep(std::shared_ptr<ScenarioStep> step)
{
    if (steps.size() == 0) step->setExecutionCondition(ExecutionTypeCondition::UNCONDITIONAL);
    steps.push_back(step);
}

bool Scenario::execute(bool isLog)
{
    ResultType prevResult = ResultType::SUCCESS;
    auto condLogger = LoggerManager::get_unique_logger();
    auto file_logger = LoggerManager::getThreadFileLogger(isLog);
    int stepN = 1;
    for (const auto& step : steps)
    {
        file_logger->info("Step {}\n", stepN, thread_id_str);
        if (isLog != false)
        {
            condLogger->info("\033[1;33mStep {}\033[0m", stepN++);
            condLogger->info("Current condition: {}\n", step->getExecutionCondition());
        }
        file_logger->info("Current condition: {}\n", step->getExecutionCondition(), thread_id_str);
        if (step->getExecutionCondition() != ExecutionTypeCondition::UNCONDITIONAL)
        {
            if (prevResult == ResultType::UNKNOWN ||
                (prevResult == ResultType::SUCCESS && step->getExecutionCondition() == ExecutionTypeCondition::FAILURE) ||
                (prevResult == ResultType::FAILURE && step->getExecutionCondition() == ExecutionTypeCondition::SUCCESS))
            {
                break;
                return false;
            }
        }
        if (step->evaluateCondition(isLog))
        {
            step->executeTask(isLog);
            prevResult = step->getExecutionResult();
        }
        else
            prevResult = ResultType::UNKNOWN;
        continue;
    }
    return true;
}

void Scenario::setName(const std::string& name)
{
    this->name = name;
}

std::string Scenario::getName() const
{
    return name;
}

void Scenario::setDescription(const std::string& description)
{
    this->description = description;
}
std::string Scenario::getDescription() const
{
    return description;
}

std::string Scenario::generateUniqueScenarioName() const
{
    static std::atomic<int> counter{0};
    std::ostringstream oss;
    oss << "scenario_" << counter++;
    return oss.str();
}
