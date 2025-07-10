#pragma once

#include <GameState.h>

class NavigationMap;

class CNavigationSystem {
public:
    CNavigationSystem(NavigationMap* pMap);

    void UpdateAgents(CGameState* pGameState, float fDt);
    void UpdateAgent(CGameObject* pGameObject, CGameState* pGameState, float fDt);
private:
    NavigationMap* m_pMap;
};