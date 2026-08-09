#pragma once

#include "../client-game-state.h"

class CEntityRespawnEvent : public IGameEvent {
public:
    CEntityRespawnEvent() {};

    virtual std::type_index GetType() const override { return typeid(CEntityRespawnEvent); };

    UnitId idUnit;
};