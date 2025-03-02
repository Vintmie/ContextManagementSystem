#include "Task/ChangePowerPlanTask.h"
#include "FormatOutput.h"
#include <windows.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Utils.h"

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

    std::wistringstream stream(output);
    std::wstring line;
    while (std::getline(stream, line))
    {
        if (line.find(planName) != std::wstring::npos && line.find(L"Power Scheme GUID:") != std::wstring::npos)
        {
            size_t startPos = line.find(L":");
            if (startPos != std::wstring::npos)
            {
                std::wstring guid = line.substr(startPos + 2);
                return guid;
            }
        }
    }

    return L"";
}

std::vector<std::wstring> GetAvailablePowerPlans()
{
    std::vector<std::wstring> powerPlans;
    std::wstring command = L"powercfg.exe /l";
    std::wstring output = ExecuteCommand(command);

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
    std::wstring planGuid = planName.substr(0, planName.find(L" "));

    std::wstring command = L"powercfg.exe /s \"" + planGuid + L"\"";

    int result = _wsystem(command.c_str());

    return (result == 0);
}

ResultType ChangePowerPlanTask::execute(bool isLog)
{
    return changePowerPlan(isLog);
}

ResultType ChangePowerPlanTask::getExecutionResult() const
{
    return currentExecutionResult;
}

ResultType ChangePowerPlanTask::changePowerPlan(bool isLog)
{
    auto res_logger = LoggerManager::get_unique_logger();
    auto file_logger = LoggerManager::getThreadFileLogger(isLog);
    std::vector<std::wstring> powerPlans = GetAvailablePowerPlans();

    int selection = 1;

    if (selection > 0 && selection <= powerPlans.size())
    {
        std::wstring planName = powerPlans[selection - 1];
        std::wstring planGuid = GetPowerPlanGuid(planName);
        if (!planGuid.empty())
        {
            if (SetPowerPlan(planGuid))
            {
                if (isLog != false)
                {
                    res_logger->info("Power plan changed successfully to: {}\n", Utils::wstring_to_utf8(planName));
                    res_logger->info("ChangePowerPlanTask returned: {}\n", currentExecutionResult);
                }

                file_logger->info("Power plan changed successfully to: {}\n", Utils::wstring_to_utf8(planName), thread_id_str);
                currentExecutionResult = ResultType::SUCCESS;
                file_logger->info("ChangePowerPlanTask returned: {}\n", currentExecutionResult, thread_id_str);
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
