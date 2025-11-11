#pragma once

#include "common/game/game-state.h"

#include "client-asset-manager.h"

class CSpellHitEvent : public IGameEvent {
public:
    CSpellHitEvent() {};

    virtual std::type_index GetType() const override { return typeid(CSpellHitEvent); };

    UnitId idUnit;
    HSound hSound;
};