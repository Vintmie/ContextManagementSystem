#pragma once

enum class ResultType
{
    SUCCESS,
    FAILURE,
    UNKNOWN
};

class ITask
{
public:
    virtual ~ITask() = default;
    virtual ResultType execute() = 0;
    virtual ResultType getExecutionResult() const = 0;
};
