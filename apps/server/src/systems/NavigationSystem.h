#pragma once

#include <game/server-game-state.h>
#include "common/game/game-system.h"
#include "events.h"

class NavigationMap;

class CNavigationSystem : public IGameSystem<CServerGameState> {
public:
    CNavigationSystem(NavigationMap* pMap);

    virtual void Update(CServerGameState* pGameState, float fDelta) override;
    void UpdateEntity(CServerGameState* pGameState, float fDelta, CNavigationComponent& nav);

    REGISTER_EVENT_HANDLER(CNavigationSystem, CSpellCastStartEvent, OnSpellCastStart);
    REGISTER_EVENT_HANDLER(CNavigationSystem, CMoveAttemptEvent, OnMoveAttempt);
    REGISTER_EVENT_HANDLER(CNavigationSystem, CAttackStartEvent, OnAttackStart);
    REGISTER_EVENT_HANDLER(CNavigationSystem, CDeathEvent, OnDeath);

private:
    NavigationMap* m_pMap;
};