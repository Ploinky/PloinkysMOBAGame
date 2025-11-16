#pragma once

#include "systems/system.h"
#include "events.h"
#include "components/Components.h"

class CServerGameState;

class CAttackSystem : public ISystem {
public:
    CAttackSystem();

    virtual void Update(CServerGameState* pGameState, float fDelta) override;
    virtual void Finalize(CServerGameState* pGameState) override;

    void OnAttackIntention(CServerGameState* pGameState, CAttackIntentionEvent* pEvt);
    void OnAttackHit(CServerGameState* pGameState, CAttackHitEvent* pEvt);
    void OnMoveIntention(CServerGameState* pGameState, CMoveIntentionEvent* pEvt);
    void OnSpellCastAttempt(CServerGameState* pGameState, CSpellAttemptCastEvent* pEvt);
};