#include "Conditional/TimeCondition.h"

TimeCondition::TimeCondition(int Uhour) : Uhour(Uhour) {}

bool TimeCondition::evaluate() const
{
    SYSTEMTIME systemTime;
    GetLocalTime(&systemTime);
    int hour = systemTime.wHour;
    spdlog::info("TimeCondition returns: {}\n", Uhour > systemTime.wHour ? true : false);
    return Uhour > systemTime.wHour ? true : false;
}

int TimeCondition::getHour() const
{
    return Uhour;
}