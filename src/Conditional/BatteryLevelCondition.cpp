#include "Conditional/BatteryLevelCondition.h"

BatteryLevelCondition::BatteryLevelCondition(int crucialLevel) : crucialLevel(crucialLevel) {}

bool BatteryLevelCondition::evaluate() const
{
    int currentBatteryLevel = getBatteryLevel();
    spdlog::info("getBatteryLevel returns: {}\n", currentBatteryLevel <= crucialLevel);
    return currentBatteryLevel <= crucialLevel;
}

int BatteryLevelCondition::getCrucialLevel() const
{
    return crucialLevel;
}

int BatteryLevelCondition::getBatteryLevel() const
{
    return 50;
}