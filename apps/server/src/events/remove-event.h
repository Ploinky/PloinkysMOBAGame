#pragma once

#include "game/server-game-state.h"

class CRemoveEvent : public IGameEvent {
public:
    CRemoveEvent(UnitId idTarget) : idTarget(idTarget) {};

    virtual std::type_index GetType() const override { return typeid(CRemoveEvent); };

    UnitId idTarget;
};