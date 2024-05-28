#pragma once

class IConditional
{
public:
    virtual ~IConditional() = default;
    virtual bool evaluate(bool isLog = true) const = 0;
};
