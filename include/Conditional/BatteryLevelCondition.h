#pragma once
#include "IConditional.h"

class BatteryLevelCondition : public IConditional {
public:
    BatteryLevelCondition(int crucialLevel);

    bool evaluate() const override;

    int getCrucialLevel() const;


private:
    int crucialLevel;

    int getBatteryLevel() const;
};
