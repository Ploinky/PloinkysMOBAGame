#pragma once

#include <queue>
#include <typeindex>

#include <game/components/components.h>
#include "common/game/game-system.h"
#include "common/game/game-state.h"
#include "common/data/game-data.h"

class CGameObject;
class CGameState;
class ISystem;

typedef struct {
    Vector2 vec2Pos;
    float fAngle;
} SpawnPoint_t;

class CClientGameState : public IGameState<CClientGameState> {
public:
    REGISTER_COMPONENT_TYPE(Animation)
    REGISTER_COMPONENT_TYPE(Attack)
    REGISTER_COMPONENT_TYPE(AudioEmitter)
    REGISTER_COMPONENT_TYPE(Health)
    REGISTER_COMPONENT_TYPE(Inventory)
    REGISTER_COMPONENT_TYPE(Movement)
    REGISTER_COMPONENT_TYPE(Particle)
    REGISTER_COMPONENT_TYPE(Pickupable)
    REGISTER_COMPONENT_TYPE(Renderable)
    REGISTER_COMPONENT_TYPE(SpellCast)
    REGISTER_COMPONENT_TYPE(Targetable)
    REGISTER_COMPONENT_TYPE(Team)
    REGISTER_COMPONENT_TYPE(Title)
    REGISTER_COMPONENT_TYPE(Transform)
    REGISTER_COMPONENT_TYPE(Useable)

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


private:
    NavigationMap* m_pNavMap;
};