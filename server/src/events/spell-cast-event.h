#pragma once

#include "GameState.h"
#include "SpellTargetInfo.h"
#include "components/Components.h"
#include "components/Components.h"

class CSpellCastEvent : public IGameEvent {
public:
    CSpellCastEvent(CSpellCastContext* spellCtx);

    virtual void Execute(CGameState* pGameState) override;

private:
    CSpellCastContext* m_spellCtx;
};