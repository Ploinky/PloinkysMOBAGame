#pragma once

#include "NetworkManager.h"
#include <game/server-game-state.h>
#include "common/game/game-system.h"
#include "events.h"

class CNetworkSystem : public IGameSystem<CServerGameState> {
public:
    CNetworkSystem(ServerNetworkManager* pManager);
    ~CNetworkSystem();

    void SyncGameState(CServerGameState* pGameState);

    REGISTER_EVENT_HANDLER(CNetworkSystem, CSpellCastStartEvent, OnSpellCastStart);
    REGISTER_EVENT_HANDLER(CNetworkSystem, CSpellHitEvent, OnSpellhit);
    REGISTER_EVENT_HANDLER(CNetworkSystem, CDeathEvent, OnDeath);
    REGISTER_EVENT_HANDLER(CNetworkSystem, CRespawnEvent, OnRespawn);
    REGISTER_EVENT_HANDLER(CNetworkSystem, CMoveEvent, OnMove);
    REGISTER_EVENT_HANDLER(CNetworkSystem, CMoveIntentionEvent, OnMoveIntention);
    REGISTER_EVENT_HANDLER(CNetworkSystem, CCooldownStartedEvent, OnCooldownStarted);
    REGISTER_EVENT_HANDLER(CNetworkSystem, CAttackStartEvent, OnAttackStart);
    REGISTER_EVENT_HANDLER(CNetworkSystem, CAttackHitEvent, OnAttackHit);
    REGISTER_EVENT_HANDLER(CNetworkSystem, CAttackFinishedEvent, OnAttackFinished);
    REGISTER_EVENT_HANDLER(CNetworkSystem, CPickedUpEvent, OnPickedUp);

private:
    ServerNetworkManager* m_pNetworkManager;
};