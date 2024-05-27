#include "Conditional/BatteryLevelCondition.h"
#include <windows.h>
#include <iostream>

BatteryLevelCondition::BatteryLevelCondition(int crucialLevel) : crucialLevel(crucialLevel) {}

bool BatteryLevelCondition::evaluate() const
{
    int currentBatteryLevel = getBatteryLevel();
    spdlog::info("currentBatteryLevel: {}\n", currentBatteryLevel);
    spdlog::info("getBatteryLevel returns: {}\n", currentBatteryLevel <= crucialLevel);
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


