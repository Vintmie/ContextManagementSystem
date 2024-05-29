#pragma once
#include "ITask.h"
#include <iostream>
#include "spdlog/spdlog.h"

class CaptureScreenTask : public ITask
{
public:
    CaptureScreenTask() {}
    CaptureScreenTask(std::wstring path) : filePath(path) {}
    ResultType execute(bool isLog = true) override;

    ResultType getExecutionResult() const override;

private:
    ResultType captureScreen(bool isLog);
    std::wstring filePath = L"..\\bin\\test.bmp";
    ResultType currentExecutionResult = ResultType::UNKNOWN;
};