#pragma once

#include "../game/server-game-state.h"

class CDespawnEvent : public IGameEvent {
public:
    CDespawnEvent(UnitId idTarget) : idTarget(idTarget) {};

    virtual std::type_index GetType() const override { return typeid(CDespawnEvent); };

    UnitId idTarget;
};