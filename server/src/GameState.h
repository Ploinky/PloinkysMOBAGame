#pragma once

#include <map>
#include <vector>
#include <queue>

#include "Common/PMG_Common.h"

class CGameObject;
class CGameState;

class IGameEvent {
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

    template<typename T>
	std::vector<T*> GetEvents() {
        std::vector<T*> vec = {};
		for(IGameEvent* pEvt : VecEvent) {
			if(T* pSpecific = dynamic_cast<T*>(pEvt)) {
				vec.push_back(pSpecific);
			}
		}
	
		return vec;
	}


    std::vector<IGameEvent*> VecEvent;

private:
    NavigationMap* m_pNavMap;
};