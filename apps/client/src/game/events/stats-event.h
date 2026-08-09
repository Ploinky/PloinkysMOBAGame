#pragma once

#include "../client-game-state.h"

class CStatsEvent : public IGameEvent {
public:
    CStatsEvent() {};

    virtual std::type_index GetType() const override { return typeid(CStatsEvent); };

    UnitId idUnit;
    int nHealth;
    int nMaxHealth;
};