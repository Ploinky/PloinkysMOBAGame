#pragma once

#include "../client-game-state.h"

class CSpellHitEvent : public IGameEvent {
public:
    CSpellHitEvent() {};

    virtual std::type_index GetType() const override { return typeid(CSpellHitEvent); };

    UnitId idUnit;
    std::string strSpellId;
};