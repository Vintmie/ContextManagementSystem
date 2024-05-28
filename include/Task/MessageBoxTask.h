#pragma once
#include "ITask.h"
#include <windows.h>

class MessageBoxTask : public ITask
{
public:
    MessageBoxTask() {}
    ResultType execute(bool isLog = true) override;

    ResultType getExecutionResult() const override;

private:
    ResultType getMessageBox(bool isLog);

    ResultType currentExecutionResult = ResultType::UNKNOWN;
};
