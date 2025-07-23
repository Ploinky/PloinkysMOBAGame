#pragma once

#include <map>
#include <vector>
#include <queue>

#include "Common/PMG_Common.h"

class CGameObject;
class CGameState;

class IGameCommand {
public:
    virtual void Execute(CGameState* pGameState) = 0;
};

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
    void QueueEvent(IGameCommand* pEvent) {
        m_queueEvents.emplace(pEvent);
    }

    void Update() {
        while(!m_queueEvents.empty()) {
            IGameCommand* ge = m_queueEvents.front();
            ge->Execute(this);
            delete ge;
            m_queueEvents.pop();
        }
    }
    
private:
    NavigationMap* m_pNavMap;
    std::queue<IGameCommand*> m_queueEvents;
};