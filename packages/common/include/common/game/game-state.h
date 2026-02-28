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
        comp.idUnit = idUnit;                                                                              \
        map##ComponentType##Indices[idUnit] = (int)vec##ComponentType##Components.size();                  \
        vec##ComponentType##Components.push_back(comp);                                                    \
        return &vec##ComponentType##Components.at(map##ComponentType##Indices[idUnit]);                    \
    }                                                                                                      \
    ComponentType##Component_t* Add##ComponentType(UnitId idUnit, ComponentType##Component_t comp) {     \
        comp.idUnit = idUnit;                                                                              \
        map##ComponentType##Indices[idUnit] = (int)vec##ComponentType##Components.size();                  \
        vec##ComponentType##Components.push_back(comp);                                                    \
        return &vec##ComponentType##Components.at(map##ComponentType##Indices[idUnit]);                    \
    }                                                                                                      \
    ComponentType##Component_t* Get##ComponentType(UnitId idUnit) {                                     \
        auto index = map##ComponentType##Indices.at(idUnit);                                               \
        if(index == -1) {                                                                                  \
            return nullptr;                                                                                \
        }                                                                                                  \
        return &vec##ComponentType##Components.at(index) ;                                                 \
    }                                                                                                        \
    void Remove##ComponentType(UnitId idUnit) {                                     \
        auto index = map##ComponentType##Indices.at(idUnit);                                               \
        if(index == -1) {                                                                                  \
            return;                                                                                \
        }                                                                                                  \
        vec##ComponentType##Components.erase(vec##ComponentType##Components.begin() + index) ;                                                 \
        map##ComponentType##Indices[idUnit] = -1;                                               \
    }                                                                                                       \
    std::vector<ComponentType##Component_t>& GetAll##ComponentType() {                                  \
        return vec##ComponentType##Components;                                                             \
    }                                                                                                      \
private:                                                                                                   \
    std::vector<ComponentType##Component_t> vec##ComponentType##Components;                               \
    std::vector<int> map##ComponentType##Indices = std::vector<int>(10000, -1);                                \
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

    std::vector<UnitId> vecUnits;
    template <typename T>
    static std::unordered_map<UnitId, T>& GetMap() {
        static std::unordered_map<UnitId, T> map;
        return map;
    }

    std::vector<IGameSystem<TGameState>*> m_vecGameSystems;
};