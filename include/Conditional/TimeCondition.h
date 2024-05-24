#pragma once
#include "IConditional.h"
#include <windows.h>


class TimeCondition : public IConditional {
private:
    int Uhour;
public:
    TimeCondition(int Uhour): Uhour(Uhour){}

    bool evaluate() const override {
        SYSTEMTIME systemTime;
        GetLocalTime(&systemTime);
        int hour = systemTime.wHour;

        return Uhour > systemTime.wHour ? true : false;
    }

};

