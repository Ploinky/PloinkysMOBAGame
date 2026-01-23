#pragma once

#include "game/client-game-state.h"

#include "client-asset-manager.h"

class CEntityDeathEvent : public IGameEvent {
public:
    CEntityDeathEvent() {};

    virtual std::type_index GetType() const override { return typeid(CEntityDeathEvent); };

    UnitId idUnit;
};