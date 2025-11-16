#pragma once

#include "game/server-game-state.h"

class CAttackStartEvent : public IGameEvent {
public:
    CAttackStartEvent(UnitId idAttacker, UnitId idTarget) : idAttacker(idAttacker), idTarget(idTarget) {

    }

    virtual std::type_index GetType() const override { return typeid(CAttackStartEvent); };

    UnitId idAttacker;
    UnitId idTarget;
};