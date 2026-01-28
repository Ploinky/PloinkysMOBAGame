#pragma once

#include <queue>

#include <common/game/game-state.h>

#include <components/Components.h>

#ifndef REGISTER_COMPONENT_TYPE
#define REGISTER_COMPONENT_TYPE(ComponentType) \
public:                                                                                                    \
    C##ComponentType##Component* Add##ComponentType(UnitId idUnit) {                                       \
        C##ComponentType##Component comp;                                                                  \
        comp.idUnit = idUnit;                                                                              \
        map##ComponentType##Indices[idUnit] = (int)vec##ComponentType##Components.size();                  \
        vec##ComponentType##Components.push_back(comp);                                                    \
        return &vec##ComponentType##Components.at(map##ComponentType##Indices[idUnit]);                    \
    }                                                                                                      \
    C##ComponentType##Component* Add##ComponentType(UnitId idUnit, C##ComponentType##Component comp) {     \
        comp.idUnit = idUnit;                                                                              \
        map##ComponentType##Indices[idUnit] = (int)vec##ComponentType##Components.size();                  \
        vec##ComponentType##Components.push_back(comp);                                                    \
        return &vec##ComponentType##Components.at(map##ComponentType##Indices[idUnit]);                    \
    }                                                                                                      \
    C##ComponentType##Component* Get##ComponentType##(UnitId idUnit) {                                     \
        auto index = map##ComponentType##Indices.at(idUnit);                                               \
        if(index == -1) {                                                                                  \
            return nullptr;                                                                                \
        }                                                                                                  \
        return &vec##ComponentType##Components.at(index) ;                                                 \
    }                                                                                                      \
    std::vector<C##ComponentType##Component>& GetAll##ComponentType##() {                                  \
        return vec##ComponentType##Components;                                                             \
    }                                                                                                      \
private:                                                                                                   \
    std::vector<C##ComponentType##Component> vec##ComponentType##Components;                               \
    std::vector<int> map##ComponentType##Indices = std::vector<int>(10000, -1);                                \
public:
#endif

class CGameState;

typedef struct {
    Vector2 vec2Pos;
    float fAngle;
} SpawnPoint_t;

class CServerGameState : public IGameState {
public:
    REGISTER_COMPONENT_TYPE(Movement)
    REGISTER_COMPONENT_TYPE(Character)
    REGISTER_COMPONENT_TYPE(Device)
    REGISTER_COMPONENT_TYPE(Transform)
    REGISTER_COMPONENT_TYPE(Network)
    REGISTER_COMPONENT_TYPE(Health)
    REGISTER_COMPONENT_TYPE(Team)
    REGISTER_COMPONENT_TYPE(Inventory)
    REGISTER_COMPONENT_TYPE(Intent)
    REGISTER_COMPONENT_TYPE(Ai)
    REGISTER_COMPONENT_TYPE(Navigation)
    REGISTER_COMPONENT_TYPE(BasicAttack)
    REGISTER_COMPONENT_TYPE(Pickupable)
    REGISTER_COMPONENT_TYPE(SpellCast)
    REGISTER_COMPONENT_TYPE(Useable)

    uint64_t CurrentTick = 0;
    UnitId CurrentUnitId = 0;
    
    void SetNavMap(NavigationMap* pNavMap);
    NavigationMap* GetNavMap() const;

    std::queue<IGameEvent*> VecEvent;

    unsigned int uTeam1Points = 0;
    unsigned int uTeam2Points = 0;

    std::unordered_map<Team, std::vector<SpawnPoint_t>> mapTeamSpawnPoints = {
        {Team::TEAM_1, {{Vector2 {1000, -1000}, 0.0f}}},
        {Team::TEAM_2, {{Vector2 {2000, -1000}, 180.0f}}},
    };

    UnitId SpawnUnit(const CGameData& gameData, std::string strId);
    UnitId CreateEntity();
private:
    NavigationMap* m_pNavMap;
};