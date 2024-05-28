#include "Conditional/BatteryLevelCondition.h"
#include <windows.h>
#include <iostream>
#include "FormatOutput.h"

BatteryLevelCondition::BatteryLevelCondition(int crucialLevel) : crucialLevel(crucialLevel) {}

bool BatteryLevelCondition::evaluate() const
{
    auto res_logger = LoggerManager::get_unique_logger();
    auto file_logger = LoggerManager::getFileLogger();
    int currentBatteryLevel = getBatteryLevel();
    res_logger->info("CurrentBatteryLevel: {}\n", currentBatteryLevel);
    file_logger->info("CurrentBatteryLevel: {}\n", currentBatteryLevel);
    res_logger->info("getBatteryLevel returns: {}\n", currentBatteryLevel <= crucialLevel);
    file_logger->info("getBatteryLevel returns: {}\n", currentBatteryLevel <= crucialLevel);
  
    return currentBatteryLevel <= crucialLevel;
}

int BatteryLevelCondition::getCrucialLevel() const
{
    return crucialLevel;
}

int BatteryLevelCondition::getBatteryLevel() const
{
    SYSTEM_POWER_STATUS status;
    if (GetSystemPowerStatus(&status))
    {
        return status.BatteryLifePercent;
    }
    else
    {
        std::cerr << "Failed to retrieve battery level." << std::endl;
        return -1;  // Error code
    }
}
