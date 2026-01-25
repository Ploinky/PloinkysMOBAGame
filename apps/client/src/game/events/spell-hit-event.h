#pragma once

#include "game/client-game-state.h"

#include "client-asset-manager.h"

class CSpellHitEvent : public IGameEvent {
public:
    CSpellHitEvent() {};

    virtual std::type_index GetType() const override { return typeid(CSpellHitEvent); };

    UnitId idUnit;
    std::string strSpellId;
};