#pragma once

#include <unordered_map>
#include "common/PMG_Common.h"
#include "common/game/game-system.h"
#include "common/game/game-event.h"

#ifndef REGISTER_COMPONENT_TYPE
#define REGISTER_COMPONENT_TYPE(ComponentType) \
public:                                                                                                    \
    ComponentType##Component_t* Add##ComponentType(UnitId idUnit) {                                       \
        ComponentType##Component_t comp;                                                                  \
        map##ComponentType##Components.emplace(idUnit, comp);                                                    \
        return &map##ComponentType##Components.at(idUnit);                    \
    }                                                                                                      \
    ComponentType##Component_t* Add##ComponentType(UnitId idUnit, ComponentType##Component_t comp) {     \
        map##ComponentType##Components.emplace(idUnit, comp);                                                    \
        return &map##ComponentType##Components.at(idUnit);                    \
    }                                                                                                      \
    ComponentType##Component_t* Get##ComponentType(UnitId idUnit) {                                     \
        auto it = map##ComponentType##Components.find(idUnit);                                               \
        if(it == map##ComponentType##Components.end()) {                                                                                  \
            return nullptr;                                                                                \
        }                                                                                                  \
        return &it->second;                                                 \
    }                                                                                                        \
    void Remove##ComponentType(UnitId idUnit) {                                     \
        auto it = map##ComponentType##Components.find(idUnit);                                               \
        if(it == map##ComponentType##Components.end()) {                                                                                  \
            return;                                                                                \
        }                                                                                                  \
        map##ComponentType##Components.erase(it) ;                                                 \
    }                                                                                                       \
    std::map<UnitId, ComponentType##Component_t>& GetAll##ComponentType() {                                  \
        return map##ComponentType##Components;                                                             \
    }                                                                                                      \
private:                                                                                                   \
    std::map<UnitId, ComponentType##Component_t> map##ComponentType##Components;                               \
public:
#endif


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

    template <typename T>
    static std::unordered_map<UnitId, T>& GetMap() {
        static std::unordered_map<UnitId, T> map;
        return map;
    }

    std::vector<IGameSystem<TGameState>*> m_vecGameSystems;
};