#pragma once
#include "IConditional.h"
#include <windows.h>


class TimeCondition : public IConditional {
private:
    int Uhour;
public:
    TimeCondition(int Uhour);

    bool evaluate() const override;
    int getHour() const;
};

