#include "Conditional/TimeCondition.h"
#include "FormatOutput.h"

TimeCondition::TimeCondition(int Uhour) : Uhour(Uhour) {}

bool TimeCondition::evaluate(bool isLog) const
{
    SYSTEMTIME systemTime;
    GetLocalTime(&systemTime);
    int hour = systemTime.wHour;
    auto res_logger = LoggerManager::get_unique_logger();
    auto file_logger = LoggerManager::getFileLogger();
    if (isLog != false)
    {
        res_logger->info("TimeCondition returns: {}\n", Uhour > systemTime.wHour ? true : false);
    }
    file_logger->info("TimeCondition returns: {}\n", Uhour > systemTime.wHour ? true : false);
    return Uhour > systemTime.wHour ? true : false;
}

int TimeCondition::getHour() const
{
    return Uhour;
}