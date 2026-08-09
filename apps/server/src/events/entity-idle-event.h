#pragma once

#include "../game/server-game-state.h"

class CEntityIdleEvent : public IGameEvent {
public:
    CEntityIdleEvent(UnitId idTarget) : idTarget(idTarget) {};

    virtual std::type_index GetType() const override { return typeid(CEntityIdleEvent); };

    UnitId idTarget;
};