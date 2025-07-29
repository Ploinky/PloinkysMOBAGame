#pragma once

#include "GameState.h"
#include "SpellTargetInfo.h"
#include "components/SpellCastComponent.h"

class CSpellCastStartEvent : public IGameEvent {
public:
    CSpellCastStartEvent(CSpellCastContext* pCtx);

    virtual std::type_index GetType() const override { return typeid(CSpellCastStartEvent); };

    CSpellCastContext* pCtx;
};