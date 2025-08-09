#pragma once

#include <map>
#include <unordered_map>
#include <vector>
#include <queue>
#include <typeindex>

#include "common/PMG_Common.h"

class CGameObject;
class CGameState;

class IGameEvent {
public:
  virtual ~IGameEvent() = default;
    virtual std::type_index GetType() const = 0;
};

typedef struct {
    Vector2 vec2Pos;
    float fAngle;
} SpawnPoint_t;

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

    unsigned int uTeam1Points = 0;
    unsigned int uTeam2Points = 0;

    std::unordered_map<Team, std::vector<SpawnPoint_t>> mapTeamSpawnPoints = {
        {Team::TEAM_1, {{Vector2 {1000, -1000}, 0.0f}}},
        {Team::TEAM_2, {{Vector2 {2000, -1000}, 180.0f}}},
    };

private:
    NavigationMap* m_pNavMap;
};