#pragma once

#include "system.h"
#include "events.h"

class CDeathEvent;

class CSpellSystem : public ISystem {
public:
    CSpellSystem();

    virtual void Update(CGameState* pGameState, float fDelta) override;

    void OnSpellAttemptCast(CGameState* pGameState, CSpellAttemptCastEvent* pEvt);
    void OnSpellCast(CGameState* pGameState, CSpellCastEvent* pCastEvent);
    
    void OnAttackIntention(CGameState* pGameState, CAttackIntentionEvent* pCastEvent);
    void OnDeath(CGameState* pGameState, CDeathEvent* pDeathEvent);

private:
    void TryCastSpell(CGameState* pGameState, CSpellCastContext* pSpellContext);

    void SpellHit(CGameState* pGameState, CSpellCastContext* pCtx);
};