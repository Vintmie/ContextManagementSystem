#include "UserInterface.h"
#include "Conditional/TimeCondition.h"
#include "Conditional/BatteryLevelCondition.h"
#include "Task/ChangePowerPlanTask.h"
#include "Task/MessageBoxTask.h"
#include "Task/ScheduleTask.h"
#include "Task/CaptureScreenTask.h"
#include "FSManager.h"
#include <iostream>
#include <cstdlib>
#include <vector>
#include <limits>
#include <set>
#include <chrono>
#include "FormatOutput.h"
#include "SystemInfo.h"
#include <functional>
#include <sstream>
#include <curses.h>

const int UserInterface::MAX_THREADS = SystemInfo::GetMaxThreads();

void initCurses()
{
    initscr();
    start_color();         // Enable color support
    cbreak();              // Line buffering disabled, Pass on everything to me
    noecho();              // Don't echo while we do getch
    keypad(stdscr, TRUE);  // Enable keypad input

    // Define color pairs
    init_pair(1, COLOR_WHITE, COLOR_BLACK);   // Default color pair
    init_pair(2, COLOR_BLACK, COLOR_GREEN);  // Highlighted color pair
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK); // for logo
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);   // Cute color pair 5
    init_pair(6, COLOR_CYAN, COLOR_BLACK);     // Cute color pair 2
    init_pair(7, COLOR_MAGENTA, COLOR_BLACK);  // Cute color pair 3

}


UserInterface::UserInterface()
    : scenarioManager(std::make_unique<ScenarioManager>()), scenarioPeriodicManager(std::make_unique<ScenarioManager>()),
      fsManager(std::make_unique<FSManager>()), scenarioUserBuffer(), scenarioFileBuffer(), periodicExecutionThreads(),
      stopPeriodicExecutionFlag(false)
{
    LoggerManager::initializeFile();
    initCurses();
}


void clearScreen()
{
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}



int menu()
{
    int screen_height, screen_width;
    getmaxyx(stdscr, screen_height, screen_width);
    curs_set(0);  // Hide cursor

    int window_height = 12;
    int window_width = 45;
    int window_y = (screen_height - window_height) / 2;
    int window_x = (screen_width - window_width) / 2;

    attron(COLOR_PAIR(4));

     // ASCII LOGO
    std::string p0 = "   _____   ______    ______    _   __    ___     ____     ____   ____   ";
    std::string p1 = "  / ___/  / ____/   / ____/   / | / /   /   |   / __ \\   /  _/  / __ \\";
    std::string p2 = "  \\__ \\  / /       / __/     /  |/ /   / /| |  / /_/ /   / /   / / / /";
    std::string p3 = " ___/ / / /___    / /___    / /|  /   / ___ | / _, _/  _/ /   / /_/ /   ";
    std::string p4 = "/____/  \\____/   /_____/   /_/ |_/   /_/  |_|/_/ |_|  /___/   \\____/    ";


    mvprintw(window_height - 8,window_x - 10, p0.c_str());
    mvprintw(window_height - 7, window_x - 10, p1.c_str());
    mvprintw(window_height - 6, window_x - 10, p2.c_str());
    mvprintw(window_height - 5, window_x - 10, p3.c_str());
    mvprintw(window_height - 4, window_x - 10, p4.c_str());
    
    attron(COLOR_PAIR(1));

    WINDOW* menuwin = newwin(window_height, window_width, window_y, window_x);
    box(menuwin, 0, 0);
    refresh();
    wrefresh(menuwin);
    int start_x = (window_width - 25) / 2;  // Assuming maximum item length is 25 characters
    std::string menu_items[] = {"Create New Scenario", "View Created Scenarios", "Scenario Manager", "Load Scenario From File",
        "View Loaded Scenarios", "Start Periodic Execution", "Show Running Scenarios", "Stop Selected Scenario", "Stop All Threads",
        "Exit Program"};

    int choice;
    int highlight = 0;
    while (1)
    {
    
        for (int i = 0; i < 10; i++)
        {
            if (i == highlight)
            {
                wattron(menuwin, COLOR_PAIR(2));
            }
            else
            {
                wattron(menuwin, COLOR_PAIR(1));
            }
            mvwprintw(menuwin, i + 1, start_x, menu_items[i].c_str());
            wattroff(menuwin, COLOR_PAIR(1) | COLOR_PAIR(2));
        }
        wrefresh(menuwin);

        choice = getch();

        switch (choice)
        {
            case KEY_UP:
                highlight--;
                if (highlight < 0)
                {
                    highlight = 9;
                }
                break;
            case KEY_DOWN:
                highlight++;
                if (highlight > 9)
                {
                    highlight = 0;
                }
                break;
            case 10:  // Enter key for selection
                delwin(menuwin);
                return highlight;  // Return the highlight value when Enter is pressed
            default: break;
        }
    }
}

void UserInterface::showMainMenu()
{
    int choice;
    while (1)
    {
        choice = menu();
        switch (choice)
        {
            case 0: createScenario(); break;
            case 1: viewScenarios(); break;
            case 2: executeScenario(); break;
            case 3: loadScenarioFromFile(); break;
            case 4: viewLoadedScenarios(); break;
            case 5: startPeriodicExecution(); break;
            case 6: showRunningScenarios(); break;
            case 7: stopSelectedScenario(); break;
            case 8: stopAllThreads(); break;
            case 9: exitProgram(); break;
            default: std::cout << "Неправильний вибір. Спробуйте ще раз.\n" << choice; break;
        }
    }

}


void UserInterface::startPeriodicExecution()
{
    clearScreen();
    if (scenarioUserBuffer.empty() && scenarioFileBuffer.empty())
    {
        std::cout << "Доступних сценаріїв для виконання немає\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return;
    }

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

    std::cout << "Максимальна кількість одночасно запущених сценаріїв може становити: " << MAX_THREADS << "\n";
    if (runningScenarioIds.size() >= MAX_THREADS)
    {
        std::cout << "Максимальна кількість потоків (" << MAX_THREADS << ") вже запущена.\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return;
    }

    std::cout << "Доступні сценарії для вибору:\n";
    int index = 1;
    for (const auto& scenario : uniqueScenarios)
    {
        std::cout << index << ") " << scenario->getName();
        std::cout << ": " << scenario->getDescription() << "\n";
        ++index;
    }

    int choice;
    std::cout << "Виберіть сценарій для виконання: ";
    std::cin >> choice;

    if (choice <= 0 || choice > uniqueScenarios.size())
    {
        std::cout << "Неправильний вибір. Повернення до головного меню.\n";
        return;
    }

    auto selectedScenario = uniqueScenarios[choice - 1];
    int scenarioId = selectedScenario->getId();

    if (runningScenarioIds.find(scenarioId) != runningScenarioIds.end())
    {
        std::cout << "Цей сценарій вже запущений.\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return;
    }

    if (runningScenarioIds.size() < MAX_THREADS)
    {
        scenarioPeriodicManager->addScenario(selectedScenario);

        stopPeriodicExecutionFlag = false;

        int executionInterval;
        do
        {
            std::cout << "Введіть інтервал виконання сценарію (у хвилинах): ";
            std::cin >> executionInterval;
            if (executionInterval <= 0)
            {
                std::cout << "Неправильний інтервал. Будь ласка, введіть значення більше нуля.\n";
            }
        } while (executionInterval <= 0);

        periodicExecutionThreads.emplace_back(
            [this, selectedScenario, scenarioId, executionInterval]()
            {
                auto file_logger = LoggerManager::getThreadFileLogger(false);
                std::thread::id thread_id = std::this_thread::get_id();
                std::ostringstream oss;
                oss << thread_id;
                thread_id_str = oss.str();

                std::unique_lock<std::mutex> lk(cv_m);
                while (!stopPeriodicExecutionFlag)
                {
                    file_logger->info("======= Scenario {} start\n", selectedScenario->getName(), thread_id_str);
                    selectedScenario->execute(false);
                    file_logger->info("Scenario {} end =======\n", selectedScenario->getName(), thread_id_str);
                    if (cv.wait_for(lk, std::chrono::minutes(executionInterval), [this] { return stopPeriodicExecutionFlag.load(); }))
                        break;
                }

                runningScenarioIds.erase(scenarioId);
            });

        runningScenarioIds.insert(scenarioId);

        std::cout << "Виконання сценарію розпочато. Сценарій буде виконуватись кожні " << executionInterval << " хвилин.\n";
    }
    else
    {
        std::cout << "Максимальна кількість потоків (" << MAX_THREADS << ") вже запущена.\n";
    }

    std::cout << "Натисніть Enter для повернення до головного меню...";
    std::cin.ignore(32767, '\n');
    std::cin.get();
    // menu();
}

void UserInterface::stopPeriodicExecution()
{
    {
        std::lock_guard<std::mutex> lk(cv_m);
        stopPeriodicExecutionFlag = true;
    }
    cv.notify_all();

    for (auto& thread : periodicExecutionThreads)
    {
        if (thread.joinable())
        {
            try
            {
                thread.join();
            }
            catch (const std::exception& e)
            {
                std::cerr << "Exception occurred while joining thread: " << e.what() << std::endl;
            }
        }
    }

    periodicExecutionThreads.clear();
}

std::unique_ptr<IConditional> createUserDefinedCondition(size_t condChoice)
{
    int screen_height, screen_width;
    getmaxyx(stdscr, screen_height, screen_width);
    int window_height = 13;
    if (condChoice == 1)
    {
        int level;
        clear();
        mvprintw(window_height, (screen_width - 35) / 2, "Enter critical battery level: ");
        echo();  // Enable echoing
        scanw("%d", &level);
        noecho();  // Disable echoing
        return std::make_unique<BatteryLevelCondition>(level);
    }
    else if (condChoice == 2)
    {
        int hour;
        clear();
        mvprintw(window_height, (screen_width - 35) / 2, "Enter hour (0-23): ");
        echo();  // Enable echoing
        scanw("%d", &hour);
        noecho();  // Disable echoing
        return std::make_unique<TimeCondition>(hour);
    }
    else
    {
        clear();
        mvprintw(window_height, (screen_width - 35) / 2, "Invalid choice. Please try again.");
        getch();  // Wait for user input
        return nullptr;
    }
}

std::unique_ptr<ITask> createUserDefinedTask(size_t taskChoice)
{
    curs_set(1);  // Show cursor
    int screen_height, screen_width;
    getmaxyx(stdscr, screen_height, screen_width);
    int window_height = 13;
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
        mvprintw(window_height, (screen_width - 90) / 2, "Enter the time (in seconds) after which the trigger will occur. Then select the file");
        mvprintw(window_height+1 , (screen_width - 90) / 2, "TIME: ");
        echo();                                            // Enable echoing
        scanw("%d", &sec);
        noecho();  // Disable echoing
      

        std::wstring filePath = Utils::OpenFileSelectionDialog(
            OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, L"Виберіть файл для планування", L"Exe\0*.exe\0");
        return std::make_unique<ScheduleTask>(sec, filePath);
    }
    else if (taskChoice == 4)
    {
        mvprintw(
            window_height, (screen_width - 50) / 2, "Press ENTER, then set the screenshot name.");
        getch();
        std::wstring filePath = Utils::SaveFileSelectionDialog(
            OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT, L"Вкажіть назву файлу скріншоту", L"BMP Files\0*.bmp\0");
        return std::make_unique<CaptureScreenTask>(filePath);
    }
    else
    {
        mvprintw(window_height, (screen_width - 90) / 2, "Incorrect choice. Please try again");
        return nullptr;
    }
}

void UserInterface::saveScenarioToFile(const std::shared_ptr<ScenarioManager>& scenario)
{
    int screen_height, screen_width;
    getmaxyx(stdscr, screen_height, screen_width);
    char save;
    clear();
    //mvprintw(screen_height / 2 - 2, (screen_width - 35) / 2, "Save scenario to file? (y/n): ")
    mvprintw(13, (screen_width - 40) / 2, "Save scenario to file? (y/n): ");
    echo();  // Enable echoing
    scanw("%c", &save);
    noecho();  // Disable echoing

    if (save == 'y' || save == 'Y')
    {
        std::wstring filePath = Utils::SaveFileSelectionDialog(
            OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT, L"Specify the scenario file name", L"JSON Files\0*.json\0");
        fsManager->saveScenarios(*scenario, Utils::wstring_to_utf8(filePath));
        mvprintw(14, (screen_width - 40) / 2, "Scenario saved to file: %s", Utils::wstring_to_utf8(filePath).c_str());
        mvprintw(17, (screen_width - 40) / 2, "PRESS ENTER TO RETURN TO THE MENU");
        getch();  // Wait for user input
    }
    else
    {
        mvprintw(14, (screen_width - 41) / 2, "Scenario was not saved to a file!");
        mvprintw(17, (screen_width - 41) / 2, "PRESS ENTER TO RETURN TO THE MENU");
        getch();  // Wait for user input
    }
}

void UserInterface::createScenario()
{
    keypad(stdscr, TRUE);  // Enable keypad input
    curs_set(1);           // Show cursor

    // Get screen dimensions
    int screen_height, screen_width;
    int window_height = 13;
    getmaxyx(stdscr, screen_height, screen_width);

    auto scenarioManager = std::make_shared<ScenarioManager>();
    auto scenario = std::make_shared<Scenario>();
    char name[100];
    char description[100];

    // Input name
    clear();
    mvprintw(window_height, (screen_width - 30) / 2, "ENTER SCENARIO NAME");
    move(window_height + 2, (screen_width - 20) / 2);          // Position the cursor for input
    echo();                                                // Enable echoing
    getnstr(name, sizeof(name) - 1);                       // Get user input
    noecho();                                              // Disable echoing

    // Input description
    clear();
    mvprintw(window_height, (screen_width - 35) / 2, "ENTER SCENARIO DESCRIPTION");
    move(window_height + 2, (screen_width - 20) / 2);      // Position the cursor for input
    echo();                                                // Enable echoing
    getnstr(description, sizeof(description) - 1);         // Get user input
    noecho();                                              // Disable echoing

    // Set the scenario name and description
    scenario->setName(name);
    scenario->setDescription(description);

    while (true)
    {
        clear();
        size_t condChoice = displayConditionals();             // Use the ncurses-based display method

        std::unique_ptr<IConditional> condition = createUserDefinedCondition(condChoice);
        if (!condition)
        {
            continue;
        }

        // Clear the window before displaying the next section
        clear();
        size_t taskChoice = displayTasks();  // Display the task selection menu
        
        clear();
        std::unique_ptr<ITask> task = createUserDefinedTask(taskChoice);
        if (!task)
        {
            continue;
        }

        ExecutionTypeCondition execType = ExecutionTypeCondition::UNCONDITIONAL;
        if (!scenario->getSteps().empty())
        {
            clear();
            mvprintw(screen_height / 2 - 4, (screen_width - 25) / 2, "Setting ExecutionTypeCondition");
            execType = selectExecutionTypeCondition();
        }

        auto step = std::make_shared<ScenarioStep>(std::move(condition), std::move(task), execType);
        scenario->addStep(step);

        char addAnother;
        clear();
        mvprintw(window_height, (screen_width - 50) / 2, "Add another step to the scenario? (y/n): ");
        echo();                                                // Enable echoing
        scanw(" %c", &addAnother);
        noecho();  // Disable echoing
        if (addAnother != 'y')
        {
            saveScenarioToFile(scenarioManager);
            break;
        }
    }
    scenarioManager->addScenario(scenario);
    scenarioUserBuffer.push_back(scenarioManager);

    clear();
    refresh();
    endwin();
}

void printConditionInfo(WINDOW* win, const std::shared_ptr<ScenarioStep>& step, int& y)
{
    wattron(win, COLOR_PAIR(6));
    if (const auto* timeCond = dynamic_cast<const TimeCondition*>(step->getCondition()))
    {
        mvwprintw(win, y++, 18, "TimeCondition (Hour: %d)", timeCond->getHour());
    }
    else if (const auto* batteryCond = dynamic_cast<const BatteryLevelCondition*>(step->getCondition()))
    {
        mvwprintw(win, y++, 18, "BatteryLevelCondition (Battery level: %d)", batteryCond->getCrucialLevel());
    }
    wattroff(win, COLOR_PAIR(6));
}
void printTaskInfo(WINDOW* win, const std::shared_ptr<ScenarioStep>& step, int& y)
{
    wattron(win, COLOR_PAIR(7));
    if (dynamic_cast<const ChangePowerPlanTask*>(step->getTask()))
    {
        mvwprintw(win, y++, 18, "ChangePowerPlanTask");
    }
    else if (dynamic_cast<const MessageBoxTask*>(step->getTask()))
    {
        mvwprintw(win, y++, 18, "MessageBoxTask");
    }
    else if (dynamic_cast<const ScheduleTask*>(step->getTask()))
    {
        mvwprintw(win, y++, 18, "ScheduleTask");
    }
    else if (dynamic_cast<const CaptureScreenTask*>(step->getTask()))
    {
        mvwprintw(win, y++, 18, "CaptureScreenTask");
    }
    wattroff(win, COLOR_PAIR(7));

}

void UserInterface::viewScenarios()
{

   initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);  // Enable keypad input for stdscr

    int screen_height, screen_width;
    getmaxyx(stdscr, screen_height, screen_width);
    curs_set(0);  // Hide cursor

    // Create a pad to contain all scenario information
    int pad_height = 1000;  // Arbitrary large height for the pad
    int pad_width = screen_width;
    WINDOW* pad = newpad(pad_height, pad_width);
    int y = 1;  // Start y-coordinate for printing scenario info

    mvwprintw(pad, y++, (screen_width / 2) - 10, "CREATED SCENARIOS");
    mvwprintw(pad, y++, (screen_width / 2) - 12, "USE ↓ and ↑ to scroll");

    int index = 1;
    for (const auto& sm : scenarioUserBuffer)
    {
        for (const auto& scenario : sm->getScenarios())
        {
            mvwprintw(pad, y++, 2, "#%d:", index++);
            mvwprintw(pad, y++, 6, "Name: %s", scenario->getName().c_str());
            mvwprintw(pad, y++, 6, "Description: %s", scenario->getDescription().c_str());

            bool isFirstStep = true;
            for (const auto& step : scenario->getSteps())
            {
                mvwprintw(pad, y++, 6, "- Condition: ");
                printConditionInfo(pad, step, y);
                mvwprintw(pad, y++, 6, "Tasks: ");
                printTaskInfo(pad, step, y);
                if (!isFirstStep)
                {
                    switch (step->getExecutionCondition())
                    {
                        case ExecutionTypeCondition::SUCCESS:
                            wattron(pad, COLOR_PAIR(4));
                            mvwprintw(pad, y++, 6, "Executed if the previous one was successful!");
                            wattroff(pad, COLOR_PAIR(4));
                            break;
                        case ExecutionTypeCondition::FAILURE:
                            wattron(pad, COLOR_PAIR(3));
                            mvwprintw(pad, y++, 6, "Executed if the previous one failed");
                            wattroff(pad, COLOR_PAIR(3));
                            break;
                        case ExecutionTypeCondition::UNCONDITIONAL:
                            wattron(pad, COLOR_PAIR(5));
                            mvwprintw(pad, y++, 6, "Executed under any conditions!");
                            wattroff(pad, COLOR_PAIR(5));
                            break;
                    }
                }
                isFirstStep = false;
            }
            ++y;
        }
    }

    mvwprintw(pad, y + 3, 2, "Press Enter to return to the main menu...");

    int pad_pos = 0;
    int pad_height_view = screen_height - 1;  // Viewport height
    int pad_width_view = screen_width - 1;    // Viewport width

    // Initial display of the pad
    prefresh(pad, pad_pos, 0, 0, 0, pad_height_view, pad_width_view);

    // Handle scrolling
    int ch;
    while ((ch = wgetch(stdscr)) != '\n')
    {
        switch (ch)
        {
            case KEY_UP:
                if (pad_pos > 0)
                {
                    pad_pos--;
                }
                break;
            case KEY_DOWN:
                if (pad_pos < pad_height - pad_height_view - 1)
                {
                    pad_pos++;
                }
                break;
            default: break;
        }
        prefresh(pad, pad_pos, 0, 0, 0, pad_height_view, pad_width_view);
    }

    delwin(pad);
    clear();
    refresh();
    endwin();
}

void UserInterface::executeScenario()
{
    endwin();
    clearScreen();
    if (scenarioUserBuffer.empty() && scenarioFileBuffer.empty())
    {
        std::cout << "Доступних сценаріїв для виконання немає\n";
    }
    else
    {
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
                std::cout << "" << index << ") ";
                std::cout << "\n      Назва: " << scenario->getName() << "\n";
                std::cout << "      Опис : " << scenario->getDescription() << "\n";
                bool isFirstStep = true;
                for (const auto& step : scenario->getSteps())
                {
                    std::cout << "           - Умова: ";
                    printConditionInfo(nullptr, step, index);
                    std::cout << "             Завдання: ";
                    printTaskInfo(nullptr, step, index);
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

                std::this_thread::sleep_for(std::chrono::seconds(2));
                clearScreen();
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
    clearScreen();
}

void UserInterface::stopAllThreads()
{
    {
        std::lock_guard<std::mutex> lk(cv_m);

        if (periodicExecutionThreads.empty())
        {
            std::cout << "Немає запущених сценаріїв для зупинки.\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
            return;
        }

        stopPeriodicExecutionFlag = true;
    }
    cv.notify_all();

    for (auto& thread : periodicExecutionThreads)
    {
        if (thread.joinable())
        {
            try
            {
                thread.join();
            }
            catch (const std::exception& e)
            {
                std::cerr << "Exception occurred while joining thread: " << e.what() << std::endl;
            }
        }
    }

    periodicExecutionThreads.clear();

    runningScenarioIds.clear();

    auto scenarios = scenarioPeriodicManager->getScenarios();
    for (const auto& scenario : scenarios)
    {
        scenarioPeriodicManager->removeScenario(scenario);
    }

    std::cout << "Всі сценарії зупинено.\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

void UserInterface::exitProgram()
{
    endwin();
    clearScreen();
    stopPeriodicExecution();
    std::cout << "Програма завершена.\n";
    exit(0);
}

size_t UserInterface::displayConditionals() const
{
    int screen_height, screen_width;
    getmaxyx(stdscr, screen_height, screen_width);
    curs_set(0);  // Hide cursor

    int window_height = 12;
    int window_width = 45;
    int window_y = (screen_height - window_height) / 2;
    int window_x = (screen_width - window_width) / 2;

    
    WINDOW* menuwin = newwin(window_height, window_width, window_y, window_x);
    box(menuwin, 0, 0);
    refresh();
    wrefresh(menuwin);
    int start_x = (window_width - 20) / 2;  // Assuming maximum item length is 25 characters
    mvprintw(window_height - 1, window_x + start_x, "CONDITIONAL LIST");
    std::vector<std::string> conditionals = {"BatteryLevelCondition", "TimeCondition"};

    size_t choice;
    size_t highlight = 0;

    while (true)
    {
        for (size_t i = 0; i < conditionals.size(); i++)
        {
            if (i == highlight)
            {
                wattron(menuwin, COLOR_PAIR(2));
            }
            else
            {
                wattron(menuwin, COLOR_PAIR(1));
            }
            mvwprintw(menuwin, i + 1, start_x, conditionals[i].c_str());
            wattroff(menuwin, COLOR_PAIR(1) | COLOR_PAIR(2));
        }
        wrefresh(menuwin);

        choice = getch();

        switch (choice)
        {
            case KEY_UP:
                highlight--;
                if (highlight < 0)
                {
                    highlight = conditionals.size() - 1;
                }
                break;
            case KEY_DOWN:
                highlight++;
                if (highlight >= conditionals.size())
                {
                    highlight = 0;
                }
                break;
            case 10:  // Enter key for selection
                delwin(menuwin);
                endwin();
                return highlight + 1;  // Return the highlight value when Enter is pressed
            default: break;
        }
    }
}

size_t UserInterface::displayTasks() const
{
    int screen_height, screen_width;
    getmaxyx(stdscr, screen_height, screen_width);
    curs_set(0);  // Hide cursor

    int window_height = 12;
    int window_width = 45;
    int window_y = (screen_height - window_height) / 2;
    int window_x = (screen_width - window_width) / 2;

    WINDOW* menuwin = newwin(window_height, window_width, window_y, window_x);
    box(menuwin, 0, 0);
    refresh();
    wrefresh(menuwin);
    int start_x = (window_width - 20) / 2;  // Assuming maximum item length is 25 characters
    mvprintw(window_height - 1, window_x + start_x + 5, "TASK LIST");
    std::vector<std::string> tasks = {"ChangePowerPlanTask", "MessageBoxTask", "ScheduleTask", "CaptureScreenTask"};

    size_t choice;
    size_t highlight = 0;

    while (1)
    {
        for (size_t i = 0; i < tasks.size(); i++)
        {
            if (i == highlight)
            {
                wattron(menuwin, COLOR_PAIR(2));
            }
            else
            {
                wattron(menuwin, COLOR_PAIR(1));
            }
            mvwprintw(menuwin, i + 1, start_x, tasks[i].c_str());
            wattroff(menuwin, COLOR_PAIR(1) | COLOR_PAIR(2));
        }
        wrefresh(menuwin);

        choice = getch();

        switch (choice)
        {
            case KEY_UP:
                highlight--;
                if (highlight < 0)
                {
                    highlight = tasks.size() - 1;
                }
                break;
            case KEY_DOWN:
                highlight++;
                if (highlight >= tasks.size())
                {
                    highlight = 0;
                }
                break;
            case 10:  // Enter key for selection
                delwin(menuwin);
                return highlight + 1;  // Return the highlight value when Enter is pressed
            default: break;
        }
    }
}

void UserInterface::showRunningScenarios()
{
    endwin();
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
        std::cout << "Максимальна кількість одночасно запущених сценаріїв повинна бути не більшою за: " << MAX_THREADS << "\n";
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
    // menu();
}

void UserInterface::stopSelectedScenario()
{
    endwin();
    {
        std::lock_guard<std::mutex> lk(cv_m);
        stopPeriodicExecutionFlag = true;
    }
    cv.notify_all();

    bool threadStopped = false;

    for (auto& thread : periodicExecutionThreads)
    {
        if (thread.joinable())
        {
            thread.join();
            threadStopped = true;
            break;
        }
    }

    if (!threadStopped)
    {
        std::cout << "Запущених сценаріїв немає.\n";
    }
    else
    {
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

    //std::cout << "Натисніть Enter для повернення до головного меню...";
    //std::cin.ignore(32767, '\n');
    //std::cin.get();
    // menu();
}

ExecutionTypeCondition UserInterface::selectExecutionTypeCondition() const
{
    int screen_height, screen_width;
    getmaxyx(stdscr, screen_height, screen_width);
    curs_set(0);  // Hide cursor

    int window_height = 7;  // Adjust this as needed
    int window_width = 45;
    int window_y = (screen_height - window_height) / 2;
    int window_x = (screen_width - window_width) / 2;

    WINDOW* menuwin = newwin(window_height, window_width, window_y, window_x);
    box(menuwin, 0, 0);
    refresh();
    wrefresh(menuwin);
    int start_x = (window_width - 10) / 2;  // Adjust this as needed
    //mvprintw(window_y - 2, window_x + start_x, "EXECUTION TYPE LIST");

    std::vector<std::string> executionTypes = {"SUCCESS", "FAILURE", "UNCONDITIONAL"};

    size_t choice;
    size_t highlight = 0;

    while (true)
    {
        for (size_t i = 0; i < executionTypes.size(); i++)
        {
            if (i == highlight)
            {
                wattron(menuwin, COLOR_PAIR(2));
            }
            else
            {
                wattron(menuwin, COLOR_PAIR(1));
            }
            mvwprintw(menuwin, i + 1, start_x, executionTypes[i].c_str());
            wattroff(menuwin, COLOR_PAIR(1) | COLOR_PAIR(2));
        }
        wrefresh(menuwin);

        choice = getch();

        switch (choice)
        {
            case KEY_UP:
                highlight--;
                if (highlight < 0)
                {
                    highlight = executionTypes.size() - 1;
                }
                break;
            case KEY_DOWN:
                highlight++;
                if (highlight >= executionTypes.size())
                {
                    highlight = 0;
                }
                break;
            case 10:  // Enter key for selection
                delwin(menuwin);
                switch (highlight)
                {
                    case 0: return ExecutionTypeCondition::SUCCESS;
                    case 1: return ExecutionTypeCondition::FAILURE;
                    case 2: return ExecutionTypeCondition::UNCONDITIONAL;
                }
            default: break;
        }
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
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);  // Enable keypad input for stdscr

    int screen_height, screen_width;
    getmaxyx(stdscr, screen_height, screen_width);
    curs_set(0);  // Hide cursor

    // Create a pad to contain all scenario information
    int pad_height = 1000;  // Arbitrary large height for the pad
    int pad_width = screen_width;
    WINDOW* pad = newpad(pad_height, pad_width);
    int y = 1;  // Start y-coordinate for printing scenario info

    mvwprintw(pad, y++, (screen_width / 2) - 10, "LOADED SCENARIOS");
    mvwprintw(pad, y++, (screen_width / 2) - 12, "USE ↓ and ↑ to scroll");

    int index = 1;
    for (const auto& sm : scenarioFileBuffer)
    {
        for (const auto& scenario : sm->getScenarios())
        {
            mvwprintw(pad, y++, 2, "#%d:", index++);
            mvwprintw(pad, y++, 6, "Name: %s", scenario->getName().c_str());
            mvwprintw(pad, y++, 6, "Description: %s", scenario->getDescription().c_str());

            bool isFirstStep = true;
            for (const auto& step : scenario->getSteps())
            {
                mvwprintw(pad, y++, 6, "- Condition: ");
                printConditionInfo(pad, step, y);
                mvwprintw(pad, y++, 6, "Tasks: ");
                printTaskInfo(pad, step, y);
                if (!isFirstStep)
                {
                    switch (step->getExecutionCondition())
                    {
                        case ExecutionTypeCondition::SUCCESS:
                            wattron(pad, COLOR_PAIR(4));
                            mvwprintw(pad, y++, 6, "Executed if the previous one was successful!");
                            wattroff(pad, COLOR_PAIR(4));
                            break;
                        case ExecutionTypeCondition::FAILURE:
                            wattron(pad, COLOR_PAIR(3));
                            mvwprintw(pad, y++, 6, "Executed if the previous one failed");
                            wattroff(pad, COLOR_PAIR(3));
                            break;
                        case ExecutionTypeCondition::UNCONDITIONAL:
                            wattron(pad, COLOR_PAIR(5));
                            mvwprintw(pad, y++, 6, "Executed under any conditions!");
                            wattroff(pad, COLOR_PAIR(5));
                            break;
                    }
                }
                isFirstStep = false;
            }
            ++y;
        }
    }

    mvwprintw(pad, y + 3, 2, "Press Enter to return to the main menu...");

    int pad_pos = 0;
    int pad_height_view = screen_height - 1;  // Viewport height
    int pad_width_view = screen_width - 1;    // Viewport width

    // Initial display of the pad
    prefresh(pad, pad_pos, 0, 0, 0, pad_height_view, pad_width_view);

    // Handle scrolling
    int ch;
    while ((ch = wgetch(stdscr)) != '\n')
    {
        switch (ch)
        {
            case KEY_UP:
                if (pad_pos > 0)
                {
                    pad_pos--;
                }
                break;
            case KEY_DOWN:
                if (pad_pos < pad_height - pad_height_view - 1)
                {
                    pad_pos++;
                }
                break;
            default: break;
        }
        prefresh(pad, pad_pos, 0, 0, 0, pad_height_view, pad_width_view);
    }

    delwin(pad);
    clear();
    refresh();
    endwin();
}