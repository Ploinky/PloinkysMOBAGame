#pragma once

#include <map>
#include <vector>
#include <queue>
#include <typeindex>

#include "Common/PMG_Common.h"

class CGameObject;
class CGameState;

class IGameEvent {
public:
  virtual ~IGameEvent() = default;
    virtual std::type_index GetType() const = 0;
};

template <typename Derived>
class BaseGameEvent : public IGameEvent {
public:
    std::type_index GetType() const override {
        return typeid(Derived);
    }
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

    std::queue<IGameEvent*> VecEvent;

private:
    NavigationMap* m_pNavMap;
};