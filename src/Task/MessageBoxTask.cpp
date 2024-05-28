#include "Task/MessageBoxTask.h"
#include "FormatOutput.h"

ResultType MessageBoxTask::execute(bool isLog)
{
    return getMessageBox(isLog);
}

ResultType MessageBoxTask::getExecutionResult() const
{
    return currentExecutionResult;
}

ResultType MessageBoxTask::getMessageBox(bool isLog)
{

    if (MessageBoxW(NULL, L"Hello from MessageBox!", L"Message", MB_OK | MB_ICONINFORMATION))
    {
        auto res_logger = LoggerManager::get_unique_logger();
        auto file_logger = LoggerManager::getFileLogger(isLog);
        currentExecutionResult = ResultType::SUCCESS;
        if (isLog != false)
        {
            res_logger->info("MessageBoxTask returned: {}\n", currentExecutionResult);
        }
        file_logger->info("MessageBoxTask returned: {}\n", currentExecutionResult);
        return currentExecutionResult;
    }
    return currentExecutionResult;
}
