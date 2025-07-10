#include "GameState.h"

#include "GameObject/GameObject.h"

std::vector<CGameObject*> CGameState::GetGameObjectsInArea(Vector3 vec3Center, float fRadius) const {
    throw std::exception("implement me");
    return {};
}

CGameObject* CGameState::FindGameObjectById(UnitId idUnit) const {
    std::map<UnitId, CGameObject*>::const_iterator entry = GameObjects.find(idUnit);

    if(entry == GameObjects.end()) {
        return nullptr;
    }

    return entry->second;
}

void CGameState::SetNavMap(NavigationMap* pNavMap) {
    m_pNavMap = pNavMap;
}

NavigationMap* CGameState::GetNavMap() const {
    return m_pNavMap;
}