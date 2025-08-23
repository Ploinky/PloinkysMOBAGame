#pragma once

#include <NetworkManagerEnet.h>
#include <GameState.h>
#include "system.h"
#include "events.h"

class CNetworkSystem : public ISystem{
public:
    CNetworkSystem(ServerNetworkManager* pManager);
    ~CNetworkSystem();

    void SyncGameState(CGameState* pGameState);

    void OnSpellCastStart(CGameState* pGameState, CSpellCastStartEvent* pEvt);
    void OnSpellhit(CGameState* pGameState, CSpellHitEvent* pEvt);
    void OnDeath(CGameState* pGameState, CDeathEvent* pEvt);
    void OnRespawn(CGameState* pGameState, CRespawnEvent* pEvt);
    void OnMove(CGameState* pGameState, CMoveEvent* pEvt);
    void OnMoveIntention(CGameState* pGameState, CMoveIntentionEvent* pEvt);
    void OnCooldownStarted(CGameState* pGameState, CCooldownStartedEvent* pEvt);
    void OnAttackStart(CGameState* pGameState, CAttackStartEvent* pEvt);
    void OnAttackHit(CGameState* pGameState, CAttackHitEvent* pEvt);
    void OnAttackFinished(CGameState* pGameState, CAttackFinishedEvent* pEvt);

private:
    ServerNetworkManager* m_pNetworkManager;
};