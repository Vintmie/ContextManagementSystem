#pragma once

class IConditional
{
public:
    virtual ~IConditional() = default;
    virtual bool evaluate() const = 0;
};
