#pragma once

#include <NetworkManager.h>
#include <GameState.h>
#include "system.h"

class CSpellCastStartEvent;
class CSpellHitEvent;
class CDeathEvent;
class CRespawnEvent;

class CNetworkSystem : public ISystem{
public:
    CNetworkSystem(ServerNetworkManager* pManager);
    ~CNetworkSystem();

    void SyncGameState(CGameState* pGameState);

    
    void OnSpellCastStart(CGameState* pGameState, CSpellCastStartEvent* pEvt);
    void OnSpellhit(CGameState* pGameState, CSpellHitEvent* pEvt);
    void OnDeath(CGameState* pGameState, CDeathEvent* pEvt);
    void OnRespawn(CGameState* pGameState, CRespawnEvent* pEvt);

private:
    ServerNetworkManager* m_pNetworkManager;
};