#pragma once

#include "GameState.h"

class CAttackFinishedEvent : public IGameEvent {
public:
    CAttackFinishedEvent(UnitId idAttacker) : idAttacker(idAttacker) {}

    virtual std::type_index GetType() const override { return typeid(CAttackFinishedEvent); };

    UnitId idAttacker;
};