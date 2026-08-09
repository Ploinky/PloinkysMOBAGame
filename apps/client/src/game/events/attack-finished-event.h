#pragma once

#include "../client-game-state.h"

class CAttackFinishedEvent : public IGameEvent {
public:
    CAttackFinishedEvent() {};

    virtual std::type_index GetType() const override { return typeid(CAttackFinishedEvent); };

    UnitId idUnit;
};