#include "server-game-state.h"

#include "GameObject.h"

std::vector<CGameObject*> CServerGameState::GetGameObjectsInArea(Vector3 vec3Center, float fRadius) const {
    throw std::runtime_error("implement me");
    return {};
}

CGameObject* CServerGameState::FindGameObjectById(UnitId idUnit) const {
    std::map<UnitId, CGameObject*>::const_iterator entry = GameObjects.find(idUnit);

    if(entry == GameObjects.end()) {
        return nullptr;
    }

    return entry->second;
}

void CServerGameState::SetNavMap(NavigationMap* pNavMap) {
    m_pNavMap = pNavMap;
}

NavigationMap* CServerGameState::GetNavMap() const {
    return m_pNavMap;
}