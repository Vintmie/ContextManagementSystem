#include "Task/ChangePowerPlanTask.h"
#include "FormatOutput.h"
#include <windows.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Utils.h"

// Function to execute a command and capture its output
std::wstring ExecuteCommand(const std::wstring& command)
{
    std::wstring result;
    FILE* pipe = _wpopen(command.c_str(), L"rt");

    if (pipe != nullptr)
    {
        wchar_t buffer[128];
        while (fgetws(buffer, 128, pipe))
        {
            result += buffer;
        }
        _pclose(pipe);
    }

    return result;
}

std::wstring GetPowerPlanGuid(const std::wstring& planName)
{
    std::wstring command = L"powercfg.exe /l";
    std::wstring output = ExecuteCommand(command);

    // Find the line containing the specified power plan name and extract the GUID
    std::wistringstream stream(output);
    std::wstring line;
    while (std::getline(stream, line))
    {
        if (line.find(planName) != std::wstring::npos && line.find(L"Power Scheme GUID:") != std::wstring::npos)
        {
            size_t startPos = line.find(L":");
            if (startPos != std::wstring::npos)
            {
                std::wstring guid = line.substr(startPos + 2);  // Extract GUID substring
                return guid;
            }
        }
    }

    return L"";
}

// Function to get a list of available power plans
std::vector<std::wstring> GetAvailablePowerPlans()
{
    std::vector<std::wstring> powerPlans;
    std::wstring command = L"powercfg.exe /l";
    std::wstring output = ExecuteCommand(command);

    // Parse the output to extract power plan names
    std::wistringstream stream(output);
    std::wstring line;
    while (std::getline(stream, line))
    {
        if (line.find(L"Power Scheme GUID:") != std::wstring::npos)
        {
            size_t startPos = line.find(L"(");
            size_t endPos = line.find(L")", startPos);
            if (startPos != std::wstring::npos && endPos != std::wstring::npos)
            {
                std::wstring planName = line.substr(startPos + 1, endPos - startPos - 1);
                powerPlans.push_back(planName);
            }
        }
    }

    return powerPlans;
}

bool SetPowerPlan(const std::wstring& planName)
{
    // Extract the GUID portion from the power plan name
    std::wstring planGuid = planName.substr(0, planName.find(L" "));

    // Construct the command to change the power plan
    std::wstring command = L"powercfg.exe /s \"" + planGuid + L"\"";

    // Print the command for manual execution
    // std::wcout << "Command to manually execute: " << command << std::endl;

    // Execute the command using system() function
    int result = _wsystem(command.c_str());

    return (result == 0);  // 0 indicates success
}

ResultType ChangePowerPlanTask::execute()
{
    return changePowerPlan();
}

ResultType ChangePowerPlanTask::getExecutionResult() const
{
    return currentExecutionResult;
}

ResultType ChangePowerPlanTask::changePowerPlan()
{
    auto res_logger = LoggerManager::get_unique_logger();

    // std::cout << "Available Power Plans:" << std::endl;
    std::vector<std::wstring> powerPlans = GetAvailablePowerPlans();

    /*
    for (size_t i = 0; i < powerPlans.size(); ++i)
    {
        std::wcout << i + 1 << ". " << powerPlans[i] << std::endl;
    }
    */

    // Prompt the user to select a power plan by number
    // std::cout << "\nEnter the number of the power plan you want to select: ";

    int selection = 1;
    // std::cin >> selection;

    if (selection > 0 && selection <= powerPlans.size())
    {
        // Get the GUID of the selected power plan
        std::wstring planName = powerPlans[selection - 1];
        std::wstring planGuid = GetPowerPlanGuid(planName);
        if (!planGuid.empty())
        {
            // std::wcout << "GUID for selected power plan: " << planGuid << std::endl;
            if (SetPowerPlan(planGuid))
            {
                //std::wcout << L"Power plan changed successfully to: " << planName << std::endl;
                res_logger->info("Power plan changed successfully to: {}\n", Utils::wstring_to_utf8(planName));
                currentExecutionResult = ResultType::SUCCESS;
                res_logger->info("ChangePowerPlanTask returned: {}\n", currentExecutionResult);
            }
            else
            {
                std::wcerr << L"Failed to change power plan." << std::endl;
                currentExecutionResult = ResultType::FAILURE;
            }
        }
        else
        {
            std::cerr << "Failed to retrieve GUID for selected power plan." << std::endl;
            currentExecutionResult = ResultType::FAILURE;
        }
    }
    else
    {
        std::cerr << "Invalid selection." << std::endl;
        currentExecutionResult = ResultType::FAILURE;
    }

    return currentExecutionResult;
}
