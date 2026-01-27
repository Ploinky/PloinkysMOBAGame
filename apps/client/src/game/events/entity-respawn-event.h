#pragma once

#include "game/client-game-state.h"

#include "client-asset-manager.h"

class CEntityRespawnEvent : public IGameEvent {
public:
    CEntityRespawnEvent() {};

    virtual std::type_index GetType() const override { return typeid(CEntityRespawnEvent); };

    UnitId idUnit;
};