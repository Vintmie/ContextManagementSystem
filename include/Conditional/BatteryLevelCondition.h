#pragma once
#include "IConditional.h"

class BatteryLevelCondition : public IConditional {
public:
    BatteryLevelCondition(int crucialLevel) : crucialLevel(crucialLevel) {}

    bool evaluate() const override {
        int currentBatteryLevel = getBatteryLevel();
        return currentBatteryLevel <= crucialLevel;
    }

private:
    int crucialLevel;

    int getBatteryLevel() const {
        return 50;
    }
};
