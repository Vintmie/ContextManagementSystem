#pragma once
#include "ITask.h"
#include <windows.h>

class MessageBoxTask : public ITask {
public:
    MessageBoxTask() {}
    ResultType execute() override;

    ResultType getExecutionResult() const override;

private:
    ResultType getMessageBox();

    ResultType currentExecutionResult = ResultType::UNKNOWN;
};


