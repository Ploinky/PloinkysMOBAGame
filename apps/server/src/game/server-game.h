#ifndef _SERVER_GAME_H_
#define _SERVER_GAME_H_

#include "common/data/game-data.h"

#include "server-game-state.h"
#include "../systems/systems.h"

// Tickrate of the server in ms per tick
// Lower values require a faster server and connection
// Higher values lead to lag...
#define TICKRATE 1000.0f/60.0f

class CServerGame {
public:
    void Start(CGameData gameData, NavMesh* pNavMesh, CNetworkSystem* pNetworkSystem);
    void Input(const IGameEvent& evt);
    void Tick();
    CServerGameState* GetState();

    // TEMP
    UnitId AddPlayer();

private:
    bool m_bIsStarted = false;
    CServerGameState m_gameState;
    CGameData m_gameData;
};

#endif