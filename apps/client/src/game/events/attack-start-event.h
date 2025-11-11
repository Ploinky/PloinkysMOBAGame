#pragma once

#include "common/game/game-state.h"

#include "client-asset-manager.h"

class CAttackStartEvent : public IGameEvent {
public:
    CAttackStartEvent() {};

    virtual std::type_index GetType() const override { return typeid(CAttackStartEvent); };

    UnitId idUnit;
    HSound hSound;
};