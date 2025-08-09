#pragma once

#include <unordered_map>
#include "common/PMG_Common.h"
#include "common/game/game-system.h"

class IGameEvent {
public:
  virtual ~IGameEvent() = default;
    virtual std::type_index GetType() const = 0;
};

class CGameState {
public:
    template <typename T>
    T* GetComponent(UnitId idUnit) {
        auto& map = GetMap<T>();
        if(map.contains(idUnit)) {
            return &map.at(idUnit);
        }
        return nullptr;
    }

    template <typename T>
    T* AddComponent(UnitId idUnit) {
        auto& map = GetMap<T>();
        if(!map.contains(idUnit)) {
            map.emplace(idUnit, T());
        }

        return GetComponent<T>(idUnit);
    }

    void EmitEvent(IGameEvent* pGameEvent) {
        for(IGameSystem* pSys : m_vecGameSystems) {
            pSys->Process(this, pGameEvent);
        }
    }

    void AddSystem(IGameSystem* pGameSystem) {
        m_vecGameSystems.push_back(pGameSystem);
    }

    std::vector<UnitId> vecUnits;
    template <typename T>
    static std::unordered_map<UnitId, T>& GetMap() {
        static std::unordered_map<UnitId, T> map;
        return map;
    }


    std::vector<IGameSystem*> m_vecGameSystems;
};