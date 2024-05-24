#pragma once
#include "ITask.h"
#include <windows.h>

class MessageBoxTask : public ITask {
public:
    MessageBoxTask() {}
    ResultType execute() override {
        return getMessageBox();
    }

    ResultType getExecutionResult() const override {
        return currentExecutionResult;

    }

private:
    ResultType getMessageBox() {
        
        if (MessageBoxW(NULL, L"Hello from MessageBox!", L"Message", MB_OK | MB_ICONINFORMATION)) {
            currentExecutionResult = ResultType::SUCCESS;
            return currentExecutionResult;
        }
        return currentExecutionResult;

    }

    ResultType currentExecutionResult = ResultType::UNKNOWN;
};


