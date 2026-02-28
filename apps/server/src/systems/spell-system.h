 #pragma once

#include "common/game/game-system.h"
#include "events.h"

class CDeathEvent;

class CSpellSystem : public IGameSystem<CServerGameState> {
public:
    CSpellSystem();

    virtual void Update(CServerGameState* pGameState, float fDelta) override;

    REGISTER_EVENT_HANDLER(CSpellSystem, CSpellAttemptCastEvent, OnSpellAttemptCast)
    REGISTER_EVENT_HANDLER(CSpellSystem, CSpellCastEvent, OnSpellCast)
    REGISTER_EVENT_HANDLER(CSpellSystem, CAttackIntentionEvent, OnAttackIntention)
    REGISTER_EVENT_HANDLER(CSpellSystem, CUseEntityEvent, OnUseEntity)
    REGISTER_EVENT_HANDLER(CSpellSystem, CDeathEvent, OnDeath)

private:
    void TryCastSpell(CServerGameState* pGameState, CSpellCastContext* pSpellContext);

    void SpellHit(CServerGameState* pGameState, CSpellCastContext* pCtx);
    
    void SpellPointHit(CServerGameState* pGameState, SpellCastContext_t context);
};