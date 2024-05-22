#pragma once

class IConditional {
public:
    virtual ~IConditional() = default;
    virtual bool evaluate() const = 0;
    
    enum class ExecutionType {
        UNCONDITIONAL,
        SUCCESS,
        FAILURE
    };
    virtual IConditional::ExecutionType getExecutionType() const = 0;

};
