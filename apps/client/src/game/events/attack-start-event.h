#pragma once

#include "../client-game-state.h"

class CAttackStartEvent : public IGameEvent {
public:
    CAttackStartEvent() {};

    virtual std::type_index GetType() const override { return typeid(CAttackStartEvent); };

    UnitId idUnit;
};