#include "Task/ScheduleTask.h"
#include "FormatOutput.h"
#include <windows.h>
#include "ScheduleTask.h"
#include <iostream>
#include <taskschd.h>
#include <comdef.h>  // Include this header for _variant_t and _bstr_t

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



ResultType ScheduleTask::execute()
{
    // Calculate the future time (e.g., current time + 3600 seconds = 1 hour)
    std::cout << "Enter future time (e.g., current time + 3600 seconds = 1 hour)\n";
    unsigned int time;
    std::cin >> time;
    std::string futureTime = getFutureTime(time);
    return scheduleTask(futureTime);
}

ResultType ScheduleTask::getExecutionResult() const
{
    return currentExecutionResult;
}

ResultType ScheduleTask::scheduleTask(std::string futureTime)
{
  
    currentExecutionResult = ResultType::FAILURE;
     // Initialize COM
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        std::cerr << "COM initialization failed with error code: " << hr << std::endl;
        return currentExecutionResult;
    }

    // Create an instance of the Task Scheduler
    ITaskService* pService = NULL;
    hr = CoCreateInstance(CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&pService);
    if (FAILED(hr))
    {
        std::cerr << "Failed to create Task Scheduler instance with error code: " << hr << std::endl;
        CoUninitialize();
        return currentExecutionResult;
    }

    // Connect to the Task Scheduler
    hr = pService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
    if (FAILED(hr))
    {
        std::cerr << "Failed to connect to Task Scheduler with error code: " << hr << std::endl;
        pService->Release();
        CoUninitialize();
        return currentExecutionResult;
    }

    // Get the root folder for the Task Scheduler
    ITaskFolder* pRootFolder = NULL;
    hr = pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);
    if (FAILED(hr))
    {
        std::cerr << "Failed to get root folder for Task Scheduler with error code: " << hr << std::endl;
        pService->Release();
        CoUninitialize();
        return currentExecutionResult;
    }

    // Create a new task definition
    ITaskDefinition* pTaskDefinition = NULL;
    hr = pService->NewTask(0, &pTaskDefinition);
    if (FAILED(hr))
    {
        std::cerr << "Failed to create a new task definition with error code: " << hr << std::endl;
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
        std::cerr << "Failed to get registration info with error code: " << hr << std::endl;
        pTaskDefinition->Release();
        pRootFolder->Release();
        pService->Release();
        CoUninitialize();
        return currentExecutionResult;
    }

    hr = pRegInfo->put_Description(_bstr_t(L"SCHEDULE TASK BY WIN-MANAGER"));  // Task name
    if (FAILED(hr))
    {
        std::cerr << "Failed to set task name with error code: " << hr << std::endl;
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
        std::cerr << "Failed to get action collection with error code: " << hr << std::endl;
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
        std::cerr << "Failed to create action with error code: " << hr << std::endl;
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
        std::cerr << "Failed to get exec action interface with error code: " << hr << std::endl;
        pAction->Release();
        pActionCollection->Release();
        pRegInfo->Release();
        pTaskDefinition->Release();
        pRootFolder->Release();
        pService->Release();
        CoUninitialize();
        return currentExecutionResult;
    }

    hr = pExecAction->put_Path(_bstr_t(L"C:\\Windows\\system32\\mspaint.exe"));  // Path to your executable
    if (FAILED(hr))
    {
        std::cerr << "Failed to set action path with error code: " << hr << std::endl;
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
        std::cerr << "Failed to get trigger collection with error code: " << hr << std::endl;
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
        std::cerr << "Failed to create time trigger with error code: " << hr << std::endl;
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
        std::cerr << "Failed to get time trigger interface with error code: " << hr << std::endl;
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

    // Calculate the future time (e.g., current time + 3600 seconds = 1 hour)
    // Convert to _bstr_t
    _bstr_t bstrFutureTime(futureTime.c_str());

    hr = pTimeTrigger->put_StartBoundary(bstrFutureTime);  // Set the start time for the task
    if (FAILED(hr))
    {
        std::cerr << "Failed to set start time for the trigger with error code: " << hr << std::endl;
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
        std::cerr << "Failed to register task definition with error code: " << hr << std::endl;
        currentExecutionResult = ResultType::FAILURE;
    }
    else
    {
        std::cout << "Task scheduled successfully!" << std::endl;
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


    return currentExecutionResult;
}
