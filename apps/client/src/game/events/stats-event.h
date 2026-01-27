#pragma once

#include "game/client-game-state.h"

#include "client-asset-manager.h"

class CStatsEvent : public IGameEvent {
public:
    CStatsEvent() {};

    virtual std::type_index GetType() const override { return typeid(CStatsEvent); };

    UnitId idUnit;
    int nHealth;
    int nMaxHealth;
};