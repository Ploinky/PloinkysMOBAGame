#pragma once

#include "systems/system.h"
#include "events.h"

class CAttackSystem : public ISystem {
public:
    CAttackSystem();

    virtual void Update(CGameState* pGameState, float fDelta) override;
    virtual void Finalize(CGameState* pGameState) override;

    void OnAttackIntention(CGameState* pGameState, CAttackIntentionEvent* pEvt);
    void OnAttackHit(CGameState* pGameState, CAttackHitEvent* pEvt);
    void OnMoveIntention(CGameState* pGameState, CMoveIntentionEvent* pEvt);
    void OnSpellCastAttempt(CGameState* pGameState, CSpellAttemptCastEvent* pEvt);
};