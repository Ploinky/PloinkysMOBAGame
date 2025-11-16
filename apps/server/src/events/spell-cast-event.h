#pragma once

#include "GameState.h"
#include "SpellTargetInfo.h"
#include "components/Components.h"
#include "components/Components.h"

class CSpellCastEvent : public IGameEvent {
public:
    CSpellCastEvent(CSpellCastContext* spellCtx);

    virtual std::type_index GetType() const override { return typeid(CSpellCastEvent); };

    CSpellCastContext* m_spellCtx;
};