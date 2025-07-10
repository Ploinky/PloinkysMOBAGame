#include "NavigationSystem.h"
#include "GameState.h"
#include "Common/navigation.h"


CNavigationSystem::CNavigationSystem(NavigationMap* pMap) {
    m_pMap = pMap;
}

void CNavigationSystem::UpdateAgents(CGameState* pGameState, float fDt) {
    for(std::pair<UnitId, CGameObject*> pair : pGameState->GameObjects) {
        UpdateAgent(pair.second, pGameState, fDt);
    }
}

void CNavigationSystem::UpdateAgent(CGameObject* pGameObject, CGameState* pGameState, float fDt) {
}