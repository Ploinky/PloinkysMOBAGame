#pragma once

#include "game/server-game-state.h"

class CAttackHitEvent : public IGameEvent {
public:
    CAttackHitEvent(UnitId idAttacker, UnitId idTarget) : idAttacker(idAttacker), idTarget(idTarget) {
    }

    virtual std::type_index GetType() const override { return typeid(CAttackHitEvent); };

    UnitId idAttacker;
    UnitId idTarget;
};