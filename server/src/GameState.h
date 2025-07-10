#pragma once

#include <map>
#include <vector>

#include "Common/PMG_Common.h"

class CGameObject;

class CGameState {
public:
    uint64_t CurrentTick = 0;
    UnitId CurrentUnitId = 0;
    std::map<UnitId, CGameObject*> GameObjects;

    void SetNavMap(NavigationMap* pNavMap);
    NavigationMap* GetNavMap() const;
    // ========================================================
    std::vector<CGameObject*> GetGameObjectsInArea(Vector3 vec3Center, float fRadius) const;
    CGameObject* FindGameObjectById(UnitId idUnit) const;

private:
    NavigationMap* m_pNavMap;
};