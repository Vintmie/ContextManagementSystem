#include "Conditional/TimeCondition.h"
#include "FormatOutput.h"

TimeCondition::TimeCondition(int Uhour) : Uhour(Uhour) {}

bool TimeCondition::evaluate() const
{
    SYSTEMTIME systemTime;
    GetLocalTime(&systemTime);
    int hour = systemTime.wHour;
    auto res_logger = LoggerManager::get_unique_logger();
    res_logger->info("TimeCondition returns: {}\n", Uhour > systemTime.wHour ? true : false);
    //spdlog::info("TimeCondition returns: {}\n", Uhour > systemTime.wHour ? true : false);
    return Uhour > systemTime.wHour ? true : false;
}

int TimeCondition::getHour() const
{
    return Uhour;
}