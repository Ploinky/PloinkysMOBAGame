#pragma once

#include "common/game/game-system.h"

#include "events.h"
#include "components/Components.h"

class CServerGameState;

class CAttackSystem : public IGameSystem<CServerGameState> {
public:
    CAttackSystem();

    virtual void Update(CServerGameState* pGameState, float fDelta) override;
    virtual void Finalize(CServerGameState* pGameState) override;

    REGISTER_EVENT_HANDLER(CAttackSystem, CAttackIntentionEvent, OnAttackIntention)
    REGISTER_EVENT_HANDLER(CAttackSystem, CAttackHitEvent, OnAttackHit)
    REGISTER_EVENT_HANDLER(CAttackSystem, CMoveIntentionEvent, OnMoveIntention)
    REGISTER_EVENT_HANDLER(CAttackSystem, CSpellAttemptCastEvent, OnSpellCastAttempt)
};