#pragma once

#include <game/server-game-state.h>
#include "system.h"
#include "events.h"

class NavigationMap;

class CNavigationSystem : public ISystem {
public:
    CNavigationSystem(NavigationMap* pMap);

    virtual void Update(CServerGameState* pGameState, float fDelta) override;
    void UpdateEntity(CServerGameState* pGameState, float fDelta, CNavigationComponent& nav);

    void OnSpellCastStart(CServerGameState* pGameState, CSpellCastStartEvent* pCastStartEvent);
    void OnMoveAttempt(CServerGameState* pGameState, CMoveAttemptEvent* pMoveAttemptEvent);
    void OnAttackStart(CServerGameState* pGameState, CAttackStartEvent* pEvt);
    void OnDeath(CServerGameState* pGameState, CDeathEvent* pEvt);

private:
    NavigationMap* m_pMap;
};