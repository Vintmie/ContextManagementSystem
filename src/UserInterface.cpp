#include "UserInterface.h"
#include "Conditional/TimeCondition.h"
#include "Conditional/BatteryLevelCondition.h"
#include "Task/ChangePowerPlanTask.h"
#include "Task/MessageBoxTask.h"
#include "FSManager.h"
#include <iostream>
#include <cstdlib>
#include <limits>

UserInterface::UserInterface() : scenarioManager(std::make_unique<ScenarioManager>()) {}

void clearScreen()
{
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

void UserInterface::showMainMenu()
{
    int choice;
    do
    {
        clearScreen();
        std::cout << "1) Створити новий сценарій\n";
        std::cout << "2) Переглянути створені сценарії\n";
        std::cout << "3) Виконати сценарій\n";
        std::cout << "4) Вихід з програми\n";
        std::cin >> choice;

        clearScreen();  // Очищуємо екран перед виконанням вибраної опції

        switch (choice)
        {
            case 1: createScenario(); break;
            case 2: viewScenarios(); break;
            case 3: executeScenario(); break;
            case 4: exitProgram(); break;
            default: std::cout << "Неправильний вибір. Спробуйте ще раз.\n"; break;
        }
    } while (choice != 4);
}

std::unique_ptr<IConditional> createUserDefinedCondition(int condChoice)
{
    if (condChoice == 1)
    {
        int level;
        std::cout << "Введіть критичний рівень батареї: ";
        std::cin >> level;
        return std::make_unique<BatteryLevelCondition>(level);
    }
    else if (condChoice == 2)
    {
        int hour;
        std::cout << "Введіть годину (0-23): ";
        std::cin >> hour;
        return std::make_unique<TimeCondition>(hour);
    }
    else
    {
        std::cout << "Неправильний вибір. Спробуйте ще раз.\n";
        return nullptr;  // або обробка помилки, залежно від потреб
    }
}

// Функція для створення завдання залежно від вибору користувача
std::unique_ptr<ITask> createUserDefinedTask(int taskChoice)
{
    if (taskChoice == 1)
    {
        return std::make_unique<ChangePowerPlanTask>();
    }
    else if (taskChoice == 2)
    {
        return std::make_unique<MessageBoxTask>();
    }
    else
    {
        std::cout << "Неправильний вибір. Спробуйте ще раз.\n";
        return nullptr;  // або обробка помилки, залежно від потреб
    }
}

void UserInterface::createScenario()
{
    auto scenario = std::make_shared<Scenario>();

    while (true)
    {
        clearScreen();
        std::cout << "1.1) Вивід переліку Conditional\n";
        displayConditionals();
        int condChoice;
        std::cin >> condChoice;

        std::unique_ptr<IConditional> condition = createUserDefinedCondition(condChoice);
        if (!condition)
        {
            continue;
        }

        clearScreen();
        std::cout << "1.2) Вивід переліку TASK\n";
        displayTasks();
        int taskChoice;
        std::cin >> taskChoice;

        std::unique_ptr<ITask> task = createUserDefinedTask(taskChoice);
        if (!task)
        {
            continue;
        }

        ExecutionTypeCondition execType = ExecutionTypeCondition::UNCONDITIONAL;
        if (!scenario->getSteps().empty())
        {
            clearScreen();
            std::cout << "1.3) Становлення ExecutionTypeCondition\n";
            execType = selectExecutionTypeCondition();
        }

        auto step = std::make_shared<ScenarioStep>(std::move(condition), std::move(task), execType);
        scenario->addStep(step);

        char addAnother;
        std::cout << "Додати ще один крок до сценарію? (y/n): ";
        std::cin >> addAnother;
        if (addAnother != 'y')
        {
            break;
        }
    }

    scenarioManager->addScenario(scenario);
    scenarioBuffer.push_back(scenario);

    char save;
    std::cout << "Пропозиція зберегти сценарій у файл (y/n): ";
    std::cin >> save;
    if (save == 'y')
    {
        FSManager fsManager(filePath);
        fsManager.saveScenarios(*scenarioManager);
        std::cout << "Сценарії збережено у файл.\n";
    }
}

void printConditionInfo(const std::shared_ptr<ScenarioStep>& step)
{
    if (const auto* timeCond = dynamic_cast<const TimeCondition*>(step->getCondition()))
    {
        std::cout << "TimeCondition (Година: " << timeCond->getHour() << ")\n";
    }
    else if (const auto* batteryCond = dynamic_cast<const BatteryLevelCondition*>(step->getCondition()))
    {
        std::cout << "BatteryLevelCondition (Рівень батареї: " << batteryCond->getCrucialLevel() << ")\n";
    }
}

void printTaskInfo(const std::shared_ptr<ScenarioStep>& step)
{
    if (dynamic_cast<const ChangePowerPlanTask*>(step->getTask()))
    {
        std::cout << "ChangePowerPlanTask\n";
    }
    else if (dynamic_cast<const MessageBoxTask*>(step->getTask()))
    {
        std::cout << "MessageBoxTask\n";
    }
}

void UserInterface::viewScenarios()
{
    clearScreen();
    std::cout << "Створені сценарії:\n";
    int index = 1;
    for (const auto& scenario : scenarioBuffer)
    {
        std::cout << "Сценарій " << index++ << ":\n";
        bool isFirstStep = true;
        for (const auto& step : scenario->getSteps())
        {
            std::cout << "- Умова: ";
            printConditionInfo(step);
            std::cout << "  Завдання: ";
            printTaskInfo(step);
            if (!isFirstStep)
            {
                std::cout << "  Умова виконання: ";
                switch (step->getExecutionCondition())
                {
                    case ExecutionTypeCondition::SUCCESS: std::cout << "попередній завершився успішно\n"; break;
                    case ExecutionTypeCondition::FAILURE: std::cout << "попередній завершився невдало\n"; break;
                    case ExecutionTypeCondition::UNCONDITIONAL: std::cout << "неважливо\n"; break;
                }
            }
            isFirstStep = false;
        }
    }
    std::cout << "Натисніть Enter для повернення до головного меню...";
    std::cin.ignore(32767, '\n');
    std::cin.get();
}

void UserInterface::executeScenario()
{
    clearScreen();
    std::cout << "Доступні сценарії для виконання:\n";
    int index = 1;
    for (const auto& scenario : scenarioBuffer)
    {
        std::cout << index << ") Сценарій " << index++ << "\n";
    }

    int choice;
    std::cout << "Виберіть сценарій для виконання: ";
    std::cin >> choice;
    clearScreen();
    if (choice > 0 && choice <= scenarioBuffer.size())
    {
        scenarioBuffer[choice - 1]->execute();
        std::cout << "Сценарій виконано.\n";
    }
    else
    {
        std::cout << "Неправильний вибір.\n";
    }
    std::cout << "Натисніть Enter для повернення до головного меню...";
    std::cin.ignore(32767, '\n');
    std::cin.get();
}

void UserInterface::exitProgram()
{
    std::cout << "Програма завершена.\n";
}

void UserInterface::displayConditionals() const
{
    std::cout << "1) BatteryLevelCondition\n";
    std::cout << "2) TimeCondition\n";
}

void UserInterface::displayTasks() const
{
    std::cout << "1) ChangePowerPlanTask\n";
    std::cout << "2) MessageBoxTask\n";
}

ExecutionTypeCondition UserInterface::selectExecutionTypeCondition() const
{
    std::cout << "1) SUCCESS\n";
    std::cout << "2) FAILURE\n";
    std::cout << "3) UNCONDITIONAL\n";
    int choice;
    std::cin >> choice;

    switch (choice)
    {
        case 1: return ExecutionTypeCondition::SUCCESS;
        case 2: return ExecutionTypeCondition::FAILURE;
        case 3: return ExecutionTypeCondition::UNCONDITIONAL;
        default: std::cout << "Неправильний вибір. Використовується UNCONDITIONAL.\n"; return ExecutionTypeCondition::UNCONDITIONAL;
    }
}
