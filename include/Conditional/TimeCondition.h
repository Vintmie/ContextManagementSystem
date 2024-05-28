#pragma once
#include "IConditional.h"
#include <spdlog/spdlog.h>
#include <windows.h>

class TimeCondition : public IConditional
{
private:
    int Uhour;

public:
    TimeCondition(int Uhour);

    bool evaluate(bool isLog = true) const override;
    int getHour() const;
};
