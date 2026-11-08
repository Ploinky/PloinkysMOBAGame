#pragma once

#include <common/game/game-system.h>
#include "../game/server-game-state.h"
#include "../events.h"

class NavigationMap;

class CNavigationSystem : public IGameSystem<CServerGameState> {
public:
    CNavigationSystem(NavigationMap* pMap);

    virtual void Update(CServerGameState* pGameState, float fDelta) override;
    void UpdateEntity(CServerGameState* pGameState, float fDelta, NavigationComponent_t& nav, UnitId id);

    REGISTER_EVENT_HANDLER(CNavigationSystem, CSpellCastStartEvent, OnSpellCastStart);
    REGISTER_EVENT_HANDLER(CNavigationSystem, CMoveAttemptEvent, OnMoveAttempt);
    REGISTER_EVENT_HANDLER(CNavigationSystem, CAttackStartEvent, OnAttackStart);
    REGISTER_EVENT_HANDLER(CNavigationSystem, CEntityIdleEvent, OnUnitIdle);
    REGISTER_EVENT_HANDLER(CNavigationSystem, CDeathEvent, OnDeath);
    REGISTER_EVENT_HANDLER(CNavigationSystem, CMoveIntentionEvent, OnMoveIntention);
    REGISTER_EVENT_HANDLER(CNavigationSystem, CStopEvent, OnStop);
    REGISTER_EVENT_HANDLER(CNavigationSystem, CRespawnEvent, OnRespawn);

private:
    NavigationMap* m_pMap;

    void TryPath(CServerGameState* pGameState, UnitId id);
    void AddBlocker(CServerGameState* pGameState, UnitId id);
    void RemoveBlocker(CServerGameState* pGameState, UnitId id);
};