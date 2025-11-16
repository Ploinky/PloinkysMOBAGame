#pragma once

#include "game/server-game-state.h"

class CDeathEvent : public IGameEvent {
public:
    CDeathEvent(UnitId idTarget) : idTarget(idTarget) {};

    virtual std::type_index GetType() const override { return typeid(CDeathEvent); };

    UnitId idTarget;
};