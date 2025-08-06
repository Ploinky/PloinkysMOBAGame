#pragma once

#include "system.h"

class CSpellCastContext;
class CSpellCastStartEvent;
class CSpellAttemptCastEvent;
class CSpellCastEvent;
class CDeathEvent;

class CSpellSystem : public ISystem {
public:
    CSpellSystem();

    virtual void Update(CGameState* pGameState, float fDelta) override;

    void OnSpellAttemptCast(CGameState* pGameState, CSpellAttemptCastEvent* pEvt);
    void OnSpellCast(CGameState* pGameState, CSpellCastEvent* pCastEvent);

    void OnDeath(CGameState* pGameState, CDeathEvent* pDeathEvent);

private:
    void TryCastSpell(CGameState* pGameState, CSpellCastContext* pSpellContext);

    void SpellHit(CGameState* pGameState, CSpellCastContext* pCtx);
};