#include "Task/ScheduleTask.h"
#include "FormatOutput.h"
#include <windows.h>
#include <iostream>
#include <taskschd.h>
#include <comdef.h>  // Include this header for _variant_t and _bstr_t
#include "Utils.h"

#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")

// Function to get the current time + additional seconds as a formatted string
std::string getFutureTime(int additionalSeconds)
{
    // Get the current time
    std::time_t currentTime = std::time(nullptr);

    // Add additional seconds
    currentTime += additionalSeconds;

    // Convert to tm structure using localtime_s
    std::tm tm_result;
    localtime_s(&tm_result, &currentTime);

    // Create a buffer to hold the formatted time
    char buffer[20];  // YYYY-MM-DDTHH:MM:SS is 19 characters + 1 for null terminator
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", &tm_result);
    return std::string(buffer);
}

ResultType ScheduleTask::execute(bool isLog)
{

    std::string futureTime = getFutureTime(startUpTime);
    return scheduleTask(futureTime,isLog);
}

ResultType ScheduleTask::getExecutionResult() const
{
    return currentExecutionResult;
}

ResultType ScheduleTask::scheduleTask(std::string futureTime, bool isLog)
{

    currentExecutionResult = ResultType::FAILURE;
    auto res_logger = LoggerManager::get_unique_logger();
    auto file_logger = LoggerManager::getFileLogger();
    // Initialize COM
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        file_logger->error("COM initialization failed with error code: {}\n", hr);
        return currentExecutionResult;
    }

    // Create an instance of the Task Scheduler
    ITaskService* pService = NULL;
    hr = CoCreateInstance(CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&pService);
    if (FAILED(hr))
    {
        file_logger->error("Failed to create Task Scheduler instance with error code: {}\n", hr);
        CoUninitialize();
        return currentExecutionResult;
    }

    // Connect to the Task Scheduler
    hr = pService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
    if (FAILED(hr))
    {
        file_logger->error("Failed to connect to Task Scheduler with error code: {}\n", hr);

        pService->Release();
        CoUninitialize();
        return currentExecutionResult;
    }

    // Get the root folder for the Task Scheduler
    ITaskFolder* pRootFolder = NULL;
    hr = pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);
    if (FAILED(hr))
    {
        file_logger->error("Failed to get root folder for Task Scheduler with error code: {}\n", hr);
        pService->Release();
        CoUninitialize();
        return currentExecutionResult;
    }

    // Create a new task definition
    ITaskDefinition* pTaskDefinition = NULL;
    hr = pService->NewTask(0, &pTaskDefinition);
    if (FAILED(hr))
    {
        file_logger->error("Failed to create a new task definition with error code: {}\n", hr);
        pRootFolder->Release();
        pService->Release();
        CoUninitialize();
        return currentExecutionResult;
    }

    // Set the task name
    IRegistrationInfo* pRegInfo = NULL;
    hr = pTaskDefinition->get_RegistrationInfo(&pRegInfo);
    if (FAILED(hr))
    {
        file_logger->error("Failed to get registration info with error code: {}\n", hr);
        pTaskDefinition->Release();
        pRootFolder->Release();
        pService->Release();
        CoUninitialize();
        return currentExecutionResult;
    }

    hr = pRegInfo->put_Description(_bstr_t(L"SCHEDULE TASK BY WIN-MANAGER"));  // Task name
    if (FAILED(hr))
    {
        file_logger->error("Failed to set task name with error code: {}\n", hr);
        pRegInfo->Release();
        pTaskDefinition->Release();
        pRootFolder->Release();
        pService->Release();
        CoUninitialize();
        return currentExecutionResult;
    }

    // Create a new action (execute a program) for the task
    IActionCollection* pActionCollection = NULL;
    hr = pTaskDefinition->get_Actions(&pActionCollection);
    if (FAILED(hr))
    {
        file_logger->error("Failed to get action collection with error code: {}\n", hr);
        pRegInfo->Release();
        pTaskDefinition->Release();
        pRootFolder->Release();
        pService->Release();
        CoUninitialize();
        return currentExecutionResult;
    }

    IAction* pAction = NULL;
    hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
    if (FAILED(hr))
    {
        file_logger->error("Failed to create action with error code: {}\n", hr);
        pActionCollection->Release();
        pRegInfo->Release();
        pTaskDefinition->Release();
        pRootFolder->Release();
        pService->Release();
        CoUninitialize();
        return currentExecutionResult;
    }

    IExecAction* pExecAction = NULL;
    hr = pAction->QueryInterface(IID_IExecAction, (void**)&pExecAction);
    if (FAILED(hr))
    {
        file_logger->error("Failed to get exec action interface with error code: {}\n", hr);
        pAction->Release();
        pActionCollection->Release();
        pRegInfo->Release();
        pTaskDefinition->Release();
        pRootFolder->Release();
        pService->Release();
        CoUninitialize();
        return currentExecutionResult;
    }

    hr = pExecAction->put_Path(Utils::stdWstringToBstr(startPath));  // Path to your executable
    if (FAILED(hr))
    {
        file_logger->error("Failed to set action path with error code: {}\n", hr);
        pExecAction->Release();
        pAction->Release();
        pActionCollection->Release();
        pRegInfo->Release();
        pTaskDefinition->Release();
        pRootFolder->Release();
        pService->Release();
        CoUninitialize();
        return currentExecutionResult;
    }

    // Define the trigger for the task (run the task at a specific time)
    ITriggerCollection* pTriggerCollection = NULL;
    hr = pTaskDefinition->get_Triggers(&pTriggerCollection);
    if (FAILED(hr))
    {
        file_logger->error("Failed to get trigger collection with error code: {}\n", hr);
        pExecAction->Release();
        pAction->Release();
        pActionCollection->Release();
        pRegInfo->Release();
        pTaskDefinition->Release();
        pRootFolder->Release();
        pService->Release();
        CoUninitialize();
        return currentExecutionResult;
    }

    ITrigger* pTrigger = NULL;
    hr = pTriggerCollection->Create(TASK_TRIGGER_TIME, &pTrigger);
    if (FAILED(hr))
    {
        file_logger->error("Failed to create time trigger with error code: {}\n", hr);
        pTriggerCollection->Release();
        pExecAction->Release();
        pAction->Release();
        pActionCollection->Release();
        pRegInfo->Release();
        pTaskDefinition->Release();
        pRootFolder->Release();
        pService->Release();
        CoUninitialize();
        return currentExecutionResult;
    }

    ITimeTrigger* pTimeTrigger = NULL;
    hr = pTrigger->QueryInterface(IID_ITimeTrigger, (void**)&pTimeTrigger);
    if (FAILED(hr))
    {
        file_logger->error("Failed to get time trigger interface with error code: {}\n", hr);
        pTrigger->Release();
        pTriggerCollection->Release();
        pExecAction->Release();
        pAction->Release();
        pActionCollection->Release();
        pRegInfo->Release();
        pTaskDefinition->Release();
        pRootFolder->Release();
        pService->Release();
        CoUninitialize();
        return currentExecutionResult;
    }

    _bstr_t bstrFutureTime(futureTime.c_str());

    hr = pTimeTrigger->put_StartBoundary(bstrFutureTime);
    if (FAILED(hr))
    {
        file_logger->error("Failed to set start time for the trigger with error code: {}\n", hr);
        pTimeTrigger->Release();
        pTrigger->Release();
        pTriggerCollection->Release();
        pExecAction->Release();
        pAction->Release();
        pActionCollection->Release();
        pRegInfo->Release();
        pTaskDefinition->Release();
        pRootFolder->Release();
        pService->Release();
        CoUninitialize();
        return currentExecutionResult;
    }

    // Save the task
    IRegisteredTask* pRegisteredTask = NULL;
    hr = pRootFolder->RegisterTaskDefinition(_bstr_t(L"SCHEDULE TASK BY WIN-MANAGER"),  // Task name
        pTaskDefinition, TASK_CREATE_OR_UPDATE,
        _variant_t(),  // User credentials (empty for current user)
        _variant_t(),  // User privileges (empty for current user)
        TASK_LOGON_INTERACTIVE_TOKEN,
        _variant_t(),  // sddl (empty to inherit from parent folder)
        &pRegisteredTask);

    if (FAILED(hr))
    {
        file_logger->error("Failed to register task definition with error code: {}\n", hr);
        currentExecutionResult = ResultType::FAILURE;
    }
    else
    {
        file_logger->info("Task {} scheduled on {}\n", Utils::wstring_to_utf8(startPath), futureTime);
        if (isLog != false)
        {
            res_logger->info("Task {} scheduled on {}\n", Utils::wstring_to_utf8(startPath), futureTime);
        }
        currentExecutionResult = ResultType::SUCCESS;

        pRegisteredTask->Release();
    }

    // Release COM objects
    pTimeTrigger->Release();
    pTrigger->Release();
    pTriggerCollection->Release();
    pExecAction->Release();
    pAction->Release();
    pActionCollection->Release();
    pRegInfo->Release();
    pTaskDefinition->Release();
    pRootFolder->Release();
    pService->Release();
    CoUninitialize();

    file_logger->info("ScheduleTask returned: {}\n", currentExecutionResult);
    if (isLog != false)
    {
        res_logger->info("ScheduleTask returned: {}\n", currentExecutionResult);
    }
    return currentExecutionResult;
}
