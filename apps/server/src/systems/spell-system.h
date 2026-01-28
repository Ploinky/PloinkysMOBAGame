#pragma once

#include "system.h"
#include "events.h"

class CDeathEvent;

class CSpellSystem : public ISystem {
public:
    CSpellSystem();

    virtual void Update(CServerGameState* pGameState, float fDelta) override;

    void OnSpellAttemptCast(CServerGameState* pGameState, CSpellAttemptCastEvent* pEvt);
    void OnSpellCast(CServerGameState* pGameState, CSpellCastEvent* pCastEvent);
    
    void OnAttackIntention(CServerGameState* pGameState, CAttackIntentionEvent* pCastEvent);
    void OnDeath(CServerGameState* pGameState, CDeathEvent* pDeathEvent);
    void OnUseEntity(CServerGameState* pGameState, CUseEntityEvent* pEvt);

private:
    void TryCastSpell(CServerGameState* pGameState, CSpellCastContext* pSpellContext);

    void SpellHit(CServerGameState* pGameState, CSpellCastContext* pCtx);
    
    void SpellPointHit(CServerGameState* pGameState, SpellCastContext_t context);
};