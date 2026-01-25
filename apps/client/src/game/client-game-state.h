#pragma once

#include <queue>
#include <typeindex>

#include <game/components/components.h>
#include <game/client-game-system.h>
#include "common/data/game-data.h"

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
    }                                                                                                      \
    std::vector<ComponentType##Component_t>& GetAll##ComponentType() {                                  \
        return vec##ComponentType##Components;                                                             \
    }                                                                                                      \
private:                                                                                                   \
    std::vector<ComponentType##Component_t> vec##ComponentType##Components;                               \
    std::vector<int> map##ComponentType##Indices = std::vector<int>(10000, -1);                                \
public:
#endif

class CGameObject;
class CGameState;
class ISystem;

typedef struct {
    Vector2 vec2Pos;
    float fAngle;
} SpawnPoint_t;

class IGameEvent {
public:
  virtual ~IGameEvent() = default;
    virtual std::type_index GetType() const = 0;
};

class CClientGameState {
public:
    REGISTER_COMPONENT_TYPE(Animation)
    REGISTER_COMPONENT_TYPE(Attack)
    REGISTER_COMPONENT_TYPE(AudioEmitter)
    REGISTER_COMPONENT_TYPE(Health)
    REGISTER_COMPONENT_TYPE(Movement)
    REGISTER_COMPONENT_TYPE(Particle)
    REGISTER_COMPONENT_TYPE(Renderable)
    REGISTER_COMPONENT_TYPE(SpellCast)
    REGISTER_COMPONENT_TYPE(Targetable)
    REGISTER_COMPONENT_TYPE(Team)
    REGISTER_COMPONENT_TYPE(Title)
    REGISTER_COMPONENT_TYPE(Transform)

    uint64_t CurrentTick = 0;
    UnitId CurrentUnitId = 0;
    
    void SetNavMap(NavigationMap* pNavMap);
    NavigationMap* GetNavMap() const;

    // ========================================================
    std::vector<CGameObject*> GetGameObjectsInArea(Vector3 vec3Center, float fRadius) const;

    std::queue<IGameEvent*> VecEvent;
    std::vector<UnitId> vecUnits;

    unsigned int uTeam1Points = 0;
    unsigned int uTeam2Points = 0;

    std::unordered_map<Team, std::vector<SpawnPoint_t>> mapTeamSpawnPoints = {
        {Team::TEAM_1, {{Vector2 {1000, -1000}, 0.0f}}},
        {Team::TEAM_2, {{Vector2 {2000, -1000}, 180.0f}}},
    };


    void EmitEvent(IGameEvent* pGameEvent) {
        for(ISystem* pSys : m_vecGameSystems) {
            pSys->Process(this, pGameEvent);
        }
    }

    void AddSystem(ISystem* pGameSystem) {
        m_vecGameSystems.push_back(pGameSystem);
    }

    std::vector<ISystem*> m_vecGameSystems;
private:
    NavigationMap* m_pNavMap;
};