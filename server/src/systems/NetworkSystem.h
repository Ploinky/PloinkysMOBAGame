#pragma once

#include <NetworkManager.h>
#include <GameState.h>
#include "EventManager.h"

class CNetworkSystem {
public:
    CNetworkSystem(ServerNetworkManager* pManager);
    ~CNetworkSystem();

    void SyncGameState(CGameState* pGameState);
private:
    ServerNetworkManager* m_pNetworkManager;
    std::vector<std::pair<EventHandlerId, EEventType>> m_vecEventHandlerIds;
};