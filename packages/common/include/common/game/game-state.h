#pragma once

#include <unordered_map>
#include "common/PMG_Common.h"
#include "common/game/game-system.h"
#include "common/game/game-event.h"

template<typename TGameState>
class IGameState {
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
        for(IGameSystem<TGameState>* pSys : m_vecGameSystems) {
            pSys->Process(static_cast<TGameState*>(this), pGameEvent);
        }
    }

    void AddSystem(IGameSystem<TGameState>* pGameSystem) {
        m_vecGameSystems.push_back(pGameSystem);
    }

    std::vector<IGameSystem<TGameState>*> AllSystems() {
        return m_vecGameSystems;
    }

    std::vector<UnitId> vecUnits;
    template <typename T>
    static std::unordered_map<UnitId, T>& GetMap() {
        static std::unordered_map<UnitId, T> map;
        return map;
    }

    std::vector<IGameSystem<TGameState>*> m_vecGameSystems;
};