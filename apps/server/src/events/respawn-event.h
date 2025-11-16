#pragma once

#include "game/server-game-state.h"

class CRespawnEvent : public IGameEvent {
public:
    CRespawnEvent(UnitId idTarget) : idTarget(idTarget) {};

    virtual std::type_index GetType() const override { return typeid(CRespawnEvent); };

    UnitId idTarget;
};