#pragma once

#include "GameState.h"
#include "SpellTargetInfo.h"
#include "components/SpellCastComponent.h"

class CSpellHitEvent : public IGameEvent {
public:
    CSpellHitEvent(UnitId idTarget, std::string strSpell) : idTarget(idTarget), strSpell(strSpell) {};

    virtual std::type_index GetType() const override { return typeid(CSpellHitEvent); };

    UnitId idTarget;
    std::string strSpell;
};