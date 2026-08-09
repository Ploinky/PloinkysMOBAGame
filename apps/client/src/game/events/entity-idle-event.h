#pragma once

#include "../client-game-state.h"

class CEntityIdleEvent : public IGameEvent {
public:
    CEntityIdleEvent() {};

    virtual std::type_index GetType() const override { return typeid(CEntityIdleEvent); };

    UnitId idUnit;
};