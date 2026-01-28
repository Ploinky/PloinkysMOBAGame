#pragma once

#include "NetworkManager.h"
#include <game/server-game-state.h>
#include "system.h"
#include "events.h"

class CNetworkSystem : public ISystem{
public:
    CNetworkSystem(ServerNetworkManager* pManager);
    ~CNetworkSystem();

    void SyncGameState(CServerGameState* pGameState);

    void OnSpellCastStart(CServerGameState* pGameState, CSpellCastStartEvent* pEvt);
    void OnSpellhit(CServerGameState* pGameState, CSpellHitEvent* pEvt);
    void OnDeath(CServerGameState* pGameState, CDeathEvent* pEvt);
    void OnRespawn(CServerGameState* pGameState, CRespawnEvent* pEvt);
    void OnMove(CServerGameState* pGameState, CMoveEvent* pEvt);
    void OnMoveIntention(CServerGameState* pGameState, CMoveIntentionEvent* pEvt);
    void OnCooldownStarted(CServerGameState* pGameState, CCooldownStartedEvent* pEvt);
    void OnAttackStart(CServerGameState* pGameState, CAttackStartEvent* pEvt);
    void OnAttackHit(CServerGameState* pGameState, CAttackHitEvent* pEvt);
    void OnAttackFinished(CServerGameState* pGameState, CAttackFinishedEvent* pEvt);
    void OnPickedUp(CServerGameState* pGameState, CPickedUpEvent* pEvt);

private:
    ServerNetworkManager* m_pNetworkManager;
};