#include "UserInterface.h"
#include "Conditional/TimeCondition.h"
#include "Conditional/BatteryLevelCondition.h"
#include "Task/ChangePowerPlanTask.h"
#include "Task/MessageBoxTask.h"
#include "Task/ScheduleTask.h"
#include "FSManager.h"
#include <iostream>
#include <cstdlib>
#include <limits>
#include <set>
#include <chrono>
#include "FormatOutput.h"
#include <functional>
#include <sstream>

UserInterface::UserInterface()
    : scenarioManager(std::make_unique<ScenarioManager>()),
      scenarioPeriodicManager(std::make_unique<ScenarioManager>()),
      fsManager(std::make_unique<FSManager>()), scenarioUserBuffer(), scenarioFileBuffer(),
      periodicExecutionThreads(), stopPeriodicExecutionFlag(false)
{
    LoggerManager::initializeFile();
    //LoggerManager::initializeRegularFile();
    LoggerManager::initializeRegularFiles(6);
}

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
        std::cout << "3) Менеджер сценаріїв\n";
        std::cout << "4) Завантажити сценарій з файлу\n";
        std::cout << "5) Переглянути завантажені сценарії\n";
        std::cout << "6) Виконувати сценарій кожну хвилину\n";  // New menu option
        std::cout << "7) Переглянути запущені сценарії\n";      // New menu option
        std::cout << "8) Зупинити виконання сценарію\n";        // New menu option
        std::cout << "9) Вихід з програми\n";
        std::cin >> choice;

        clearScreen();

        switch (choice)
        {
            case 1: createScenario(); break;
            case 2: viewScenarios(); break;
            case 3: executeScenario(); break;
            case 4: loadScenarioFromFile(); break;
            case 5: viewLoadedScenarios(); break;
            case 6: startPeriodicExecution(); break;  // New case for starting periodic execution
            case 7: showRunningScenarios(); break;    // New case for starting periodic execution
            case 8: stopSelectedScenario(); break;    // New case for stopping periodic execution
            case 9: exitProgram(); break;
            default: std::cout << "Неправильний вибір. Спробуйте ще раз.\n"; break;
        }
    } while (choice != 9);
}

void UserInterface::startPeriodicExecution()
{
    clearScreen();
    if (scenarioUserBuffer.empty() && scenarioFileBuffer.empty())
    {
        std::cout << "Доступних сценаріїв для виконання немає\n";
        return;
    }

    // Об'єднання сценаріїв з уникненням дублікатів за назвою
    std::vector<std::shared_ptr<Scenario>> uniqueScenarios;
    std::set<std::string> scenarioNames;

    for (const auto& su : scenarioUserBuffer)
    {
        for (const auto& scenario : su->getScenarios())
        {
            if (scenarioNames.find(scenario->getName()) == scenarioNames.end())
            {
                uniqueScenarios.push_back(scenario);
                scenarioNames.insert(scenario->getName());
            }
        }
    }

    for (const auto& sf : scenarioFileBuffer)
    {
        for (const auto& scenario : sf->getScenarios())
        {
            if (scenarioNames.find(scenario->getName()) == scenarioNames.end())
            {
                uniqueScenarios.push_back(scenario);
                scenarioNames.insert(scenario->getName());
            }
        }
    }

    // Показ списку сценаріїв для вибору
    std::cout << "Доступні сценарії для вибору:\n";
    int index = 1;
    for (const auto& scenario : uniqueScenarios)
    {
        std::cout << index << ") " << scenario->getName();
        std::cout << ": " << scenario->getDescription() << "\n";
        ++index;
    }

    int choice;
    std::cout << "Виберіть сценарій для виконання кожну хвилину: ";
    std::cin >> choice;

    if (choice <= 0 || choice > uniqueScenarios.size())
    {
        std::cout << "Неправильний вибір. Повернення до головного меню.\n";
        return;
    }

    auto selectedScenario = uniqueScenarios[choice - 1];
    scenarioPeriodicManager->addScenario(selectedScenario);

    stopPeriodicExecutionFlag = false;

   periodicExecutionThreads.emplace_back(
        [this, selectedScenario]()
        {
            auto file_logger = LoggerManager::getThreadFileLogger(false);
            std::thread::id thread_id = std::this_thread::get_id();
            std::ostringstream oss;
            oss << thread_id;
            thread_id_str = oss.str();  // Set the thread-local thread_id_str

            std::unique_lock<std::mutex> lk(cv_m);
            while (!stopPeriodicExecutionFlag)
            {
                file_logger->info("======= Scenario {} start\n", selectedScenario->getName(), thread_id_str);
                selectedScenario->execute(false);
                file_logger->info("Scenario {} end =======\n", selectedScenario->getName(), thread_id_str);
                if (cv.wait_for(lk, std::chrono::minutes(1), [this] { return stopPeriodicExecutionFlag.load(); })) break;
            }
        });

    std::cout << "Виконання сценарію розпочато. Сценарій буде виконуватись кожну хвилину.\n";
    std::cout << "Натисніть Enter для повернення до головного меню...";
    std::cin.ignore(32767, '\n');
    std::cin.get();
}

void UserInterface::stopPeriodicExecution()
{
    {
        std::lock_guard<std::mutex> lk(cv_m);
        stopPeriodicExecutionFlag = true;
    }
    cv.notify_all();  // Notify all waiting threads to stop

    // Display the status of scenarios only once
    bool hasRunningScenarios = false;
    for (auto& thread : periodicExecutionThreads)
    {
        if (thread.joinable())
        {
            hasRunningScenarios = true;
            break;
        }
    }

    if (hasRunningScenarios)
    {
        std::cout << "Зупинено виконання сценаріїв.\n";
    }
    else
    {
        std::cout << "Немає активного виконання сценаріїв.\n";
    }

    std::cout << "Натисніть Enter для повернення до головного меню...";
    std::cin.ignore(32767, '\n');
    std::cin.get();
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
    else if (taskChoice == 3)
    {
        int sec;
        // Calculate the future time (e.g., current time + 3600 seconds = 1 hour)
        std::cout << "Введіть час (у секундах), через який відбудеться спрацювання. Після цього оберіть файл.\n";
        std::cin >> sec;
        std::wstring filePath = Utils::OpenFileSelectionDialog(
            OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, L"Виберіть файл для планування", L"Exe\0*.exe\0");
        return std::make_unique<ScheduleTask>(sec, filePath);
    }
    else
    {
        std::cout << "Неправильний вибір. Спробуйте ще раз.\n";
        return nullptr;  // або обробка помилки, залежно від потреб
    }
}

void UserInterface::saveScenarioToFile(const std::shared_ptr<ScenarioManager>& scenario)
{
    char save;
    std::cout << "Зберегти сценарій у файл (y/n): ";
    std::cin >> save;
    if (save == 'y')
    {
        std::wstring filePath = Utils::SaveFileSelectionDialog(
            OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT, L"Вкажіть назву файлу сценарію", L"JSON Files\0*.json\0");
        fsManager->saveScenarios(*scenario, Utils::wstring_to_utf8(filePath));
        std::cout << "Сценарій збережено у файл.\n";
    }
}

void UserInterface::createScenario()
{
    auto scenarioManager = std::make_shared<ScenarioManager>();
    auto scenario = std::make_shared<Scenario>();
    std::string name, description;
    std::cout << "Введіть назву сценарію: ";
    std::cin.ignore(32767, '\n');
    std::getline(std::cin, name);
    std::cout << "Введіть опис сценарію: ";
    std::getline(std::cin, description);

    scenario->setName(name);
    scenario->setDescription(description);
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
    // ЧИ СФОРМУВАТИ ІНШИЙ СЦЕНАРІЙ???
    scenarioManager->addScenario(scenario);
    scenarioUserBuffer.push_back(scenarioManager);

    // Збереження сценарію у файл
    saveScenarioToFile(scenarioManager);
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
    else if (dynamic_cast<const ScheduleTask*>(step->getTask()))
    {
        std::cout << "ScheduleTask\n";
    }
}

void UserInterface::viewScenarios()
{
    clearScreen();
    std::cout << "Створені сценарії:\n";
    int index = 1;
    for (const auto& sm : scenarioUserBuffer)
    {
        std::cout << "# " << index++ << ":\n";
        for (const auto& scenario : sm->getScenarios())
        {
            std::cout << "\n      Назва: " << scenario->getName() << "\n";
            std::cout << "      Опис : " << scenario->getDescription() << "\n";
            bool isFirstStep = true;
            for (const auto& step : scenario->getSteps())
            {
                std::cout << "           - Умова: ";
                printConditionInfo(step);
                std::cout << "             Завдання: ";
                printTaskInfo(step);
                if (!isFirstStep)
                {
                    std::cout << "            Виконується ";
                    switch (step->getExecutionCondition())
                    {
                        case ExecutionTypeCondition::SUCCESS: std::cout << "якщо попередній завершився успішно!\n"; break;
                        case ExecutionTypeCondition::FAILURE: std::cout << "якщо попередній завершився невдало!\n"; break;
                        case ExecutionTypeCondition::UNCONDITIONAL: std::cout << "за будь-яких умов!\n"; break;
                    }
                }
                isFirstStep = false;
            }
        }
    }
    std::cout << "\nНатисніть Enter для повернення до головного меню...";
    std::cin.ignore(32767, '\n');
    std::cin.get();
}

void UserInterface::executeScenario()
{
    clearScreen();
    if (scenarioUserBuffer.empty() && scenarioFileBuffer.empty())
    {
        std::cout << "Доступних сценаріїв для виконання немає\n";
    }
    else
    {
        // Об'єднання сценаріїв з уникненням дублікатів за назвою
        std::vector<std::shared_ptr<Scenario>> uniqueScenarios;
        std::set<std::string> scenarioNames;

        for (const auto& su : scenarioUserBuffer)
        {
            for (const auto& scenario : su->getScenarios())
            {
                if (scenarioNames.find(scenario->getName()) == scenarioNames.end())
                {
                    uniqueScenarios.push_back(scenario);
                    scenarioNames.insert(scenario->getName());
                }
            }
        }

        for (const auto& sf : scenarioFileBuffer)
        {
            for (const auto& scenario : sf->getScenarios())
            {
                if (scenarioNames.find(scenario->getName()) == scenarioNames.end())
                {
                    uniqueScenarios.push_back(scenario);
                    scenarioNames.insert(scenario->getName());
                }
            }
        }
        char start;
        std::cout << "Сформувати перелік сценаріїв для виконання? (y/n): ";
        std::cin >> start;
        if (start != 'y')
        {
            return;
        }
        else if (start == 'y')
        {
            clearScreen();
            std::cout << "Доступні сценарії для виконання:\n";
            int index = 1;
            for (const auto& scenario : uniqueScenarios)
            {
                std::cout << "" << index << ") " << scenario->getName();
                std::cout << ": " << scenario->getDescription() << "\n";
                std::cout << "\n      Назва: " << scenario->getName() << "\n";
                std::cout << "      Опис : " << scenario->getDescription() << "\n";
                bool isFirstStep = true;
                for (const auto& step : scenario->getSteps())
                {
                    std::cout << "           - Умова: ";
                    printConditionInfo(step);
                    std::cout << "             Завдання: ";
                    printTaskInfo(step);
                    if (!isFirstStep)
                    {
                        std::cout << "             Виконується ";
                        switch (step->getExecutionCondition())
                        {
                            case ExecutionTypeCondition::SUCCESS: std::cout << "якщо попередній завершився успішно!\n"; break;
                            case ExecutionTypeCondition::FAILURE: std::cout << "якщо попередній завершився невдало!\n"; break;
                            case ExecutionTypeCondition::UNCONDITIONAL: std::cout << "за будь-яких умов!\n"; break;
                        }
                    }
                    isFirstStep = false;
                }
                ++index;
            }
            std::vector<int> choices;
            int choice;

            std::cout << "\n\nВиберіть сценарії для виконання (введіть 0 для завершення вибору):\n";
            while (true)
            {
                std::cout << "Введіть номер сценарію: ";
                std::cin >> choice;
                if (choice == 0)
                {
                    break;
                }
                else if (choice > 0 && choice <= uniqueScenarios.size())
                {
                    choices.push_back(choice);
                }
                else
                {
                    std::cout << "Неправильний вибір. Спробуйте ще раз.\n";
                }
            }

            clearScreen();
            if (!choices.empty())
            {
                for (int ch : choices)
                {
                    scenarioManager->addScenario(uniqueScenarios[ch - 1]);
                }

                // Запит на збереження сценаріїв у файл
                char save;
                std::cout << "Зберегти сценарії у файл перед виконанням? (y/n): ";
                std::cin >> save;
                if (save == 'y')
                {

                    std::wstring filePath = Utils::SaveFileSelectionDialog(
                        OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT, L"Вкажіть назву файлу сценарію", L"JSON Files\0*.json\0");
                    // fsManager->saveScenario(*uniqueScenarios[ch - 1], Utils::wstring_to_utf8(filePath));
                    fsManager->saveScenarios(*scenarioManager, Utils::wstring_to_utf8(filePath));
                    std::cout << "Сценарії збережено у файл.\n";
                    std::this_thread::sleep_for(std::chrono::seconds(3));
                    clearScreen();
                }
                else
                {
                    std::this_thread::sleep_for(std::chrono::seconds(3));
                    clearScreen();
                }
                // Виконання сценаріїв
                scenarioManager->executeScenarios();
            }
            else
            {
                std::cout << "Немає вибраних сценаріїв для виконання.\n";
            }
        }
    }
    scenarioManager->clearScenarios();
    std::cout << "Натисніть Enter для повернення до головного меню...";
    std::cin.ignore(32767, '\n');
    std::cin.get();
}

void UserInterface::exitProgram()
{
    stopPeriodicExecution();
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
    std::cout << "3) ScheduleTask\n";
}

void UserInterface::showRunningScenarios()
{
    bool hasRunningScenarios = false;

    // Check if there are any running scenarios
    for (auto& thread : periodicExecutionThreads)
    {
        if (thread.joinable())
        {
            hasRunningScenarios = true;
            break;
        }
    }

    if (hasRunningScenarios)
    {
        std::cout << "Запущені сценарії:\n";
        int index = 1;
        for (const auto& scenario : scenarioPeriodicManager->getScenarios())
        {
            std::cout << index << ") " << scenario->getName();
            std::cout << ": " << scenario->getDescription() << "\n";
            ++index;
        }
    }
    else
    {
        std::cout << "Запущених сценаріїв немає.\n";
    }

    std::cout << "\nНатисніть Enter для повернення до головного меню...";
    std::cin.ignore(32767, '\n');
    std::cin.get();
}

void UserInterface::stopSelectedScenario()
{
    {
        std::lock_guard<std::mutex> lk(cv_m);
        stopPeriodicExecutionFlag = true;
    }
    cv.notify_all();  // Notify all waiting threads to stop

    // Flag to track if any thread was stopped
    bool threadStopped = false;

    for (auto& thread : periodicExecutionThreads)
    {
        if (thread.joinable())
        {
            thread.join();  // Stop the thread
            threadStopped = true;
            break;  // Stop the loop after stopping one thread
        }
    }

    if (!threadStopped)
    {
        std::cout << "Запущених сценаріїв немає.\n";
    }
    else
    {
        // Now remove the stopped scenario from the manager
        auto scenarios = scenarioPeriodicManager->getScenarios();

        std::cout << "Запущені сценарії:\n";
        int index = 1;
        for (const auto& scenario : scenarios)
        {
            std::cout << index << ") " << scenario->getName();
            std::cout << ": " << scenario->getDescription() << "\n";
            ++index;
        }

        int choice;
        std::cout << "Виберіть сценарій для зупинки: ";
        std::cin >> choice;

        if (choice <= 0 || choice > scenarios.size())
        {
            std::cout << "Неправильний вибір. Повернення до головного меню.\n";
            return;
        }

        auto selectedScenario = scenarios[choice - 1];
        scenarioPeriodicManager->removeScenario(selectedScenario);
        std::cout << "Виконання обраного сценарію зупинено.\n";
    }

    std::cout << "Натисніть Enter для повернення до головного меню...";
    std::cin.ignore(32767, '\n');
    std::cin.get();
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

void UserInterface::loadScenarioFromFile()
{
    std::wstring filePath = Utils::OpenFileSelectionDialog(
        OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, L"Виберіть файл сценарію", L"JSON Files\0*.json\0");
    std::shared_ptr<ScenarioManager> scenario = std::make_shared<ScenarioManager>();
    fsManager->loadScenarios(*scenario, Utils::wstring_to_utf8(filePath));
    scenarioFileBuffer.push_back(std::move(scenario));
}

void UserInterface::viewLoadedScenarios()
{
    clearScreen();
    std::cout << "Завантажені сценарії:\n";
    int index = 1;
    for (const auto& sm : scenarioFileBuffer)
    {
        std::cout << "# " << index++ << ":\n";
        for (const auto& scenario : sm->getScenarios())
        {
            std::cout << "\n      Назва: " << scenario->getName() << "\n";
            std::cout << "      Опис : " << scenario->getDescription() << "\n";
            bool isFirstStep = true;
            for (const auto& step : scenario->getSteps())
            {
                std::cout << "           - Умова: ";
                printConditionInfo(step);
                std::cout << "             Завдання: ";
                printTaskInfo(step);
                if (!isFirstStep)
                {
                    std::cout << "            Виконується ";
                    switch (step->getExecutionCondition())
                    {
                        case ExecutionTypeCondition::SUCCESS: std::cout << "якщо попередній завершився успішно!\n"; break;
                        case ExecutionTypeCondition::FAILURE: std::cout << "якщо попередній завершився невдало!\n"; break;
                        case ExecutionTypeCondition::UNCONDITIONAL: std::cout << "за будь-яких умов!\n"; break;
                    }
                }
                isFirstStep = false;
            }
        }
    }
    std::cout << "\nНатисніть Enter для повернення до головного меню...";
    std::cin.ignore(32767, '\n');
    std::cin.get();
}