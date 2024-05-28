#include "Task/MessageBoxTask.h"
#include "FormatOutput.h"

ResultType MessageBoxTask::execute()
{
    return getMessageBox();
}

ResultType MessageBoxTask::getExecutionResult() const
{
    return currentExecutionResult;
}

ResultType MessageBoxTask::getMessageBox()
{

    if (MessageBoxW(NULL, L"Hello from MessageBox!", L"Message", MB_OK | MB_ICONINFORMATION))
    {
        auto res_logger = LoggerManager::get_unique_logger();
        currentExecutionResult = ResultType::SUCCESS;
        res_logger->info("MessageBoxTask returned: {}\n", currentExecutionResult);
        return currentExecutionResult;
    }
    return currentExecutionResult;
}
