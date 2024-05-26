#include "Conditional/TimeCondition.h"

TimeCondition::TimeCondition(int Uhour): Uhour(Uhour){}

bool TimeCondition::evaluate() const {
        SYSTEMTIME systemTime;
        GetLocalTime(&systemTime);
        int hour = systemTime.wHour;

        return Uhour > systemTime.wHour ? true : false;
    }

int TimeCondition::getHour() const { return Uhour; }