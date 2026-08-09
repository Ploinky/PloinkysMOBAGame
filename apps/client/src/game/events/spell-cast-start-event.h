#pragma once

#include "../client-game-state.h"

class CSpellCastStartEvent : public IGameEvent {
public:
    CSpellCastStartEvent() {};

    virtual std::type_index GetType() const override { return typeid(CSpellCastStartEvent); };

    UnitId idUnit;
    std::string strSpellId;
};