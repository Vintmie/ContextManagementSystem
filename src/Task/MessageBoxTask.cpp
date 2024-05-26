#include "Task/MessageBoxTask.h"

ResultType MessageBoxTask::execute() {
        return getMessageBox();
}

ResultType MessageBoxTask::getExecutionResult() const {
        return currentExecutionResult;
}

ResultType MessageBoxTask::getMessageBox() {
        
        if (MessageBoxW(NULL, L"Hello from MessageBox!", L"Message", MB_OK | MB_ICONINFORMATION)) {
            currentExecutionResult = ResultType::SUCCESS;
            return currentExecutionResult;
        }
        return currentExecutionResult;

}
