#pragma once

#include "game/client-game-state.h"

#include "client-asset-manager.h"

class CSpellCastStartEvent : public IGameEvent {
public:
    CSpellCastStartEvent() {};

    virtual std::type_index GetType() const override { return typeid(CSpellCastStartEvent); };

    UnitId idUnit;
    std::string strSpellId;
};