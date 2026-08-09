#pragma once

#include "game/client-game-state.h"

#include "client-asset-manager.h"

class CAttackFinishedEvent : public IGameEvent {
public:
    CAttackFinishedEvent() {};

    virtual std::type_index GetType() const override { return typeid(CAttackFinishedEvent); };

    UnitId idUnit;
};