#pragma once

#include <queue>

#include <common/game/game-state.h>

#include <components/Components.h>


#ifndef REGISTER_EVENT_HANDLER
#define REGISTER_EVENT_HANDLER(EventType, Method) \
    RegisterHandler<EventType>([this](CServerGameState* state, EventType* e) { this->Method(state, e); });
#endif

class CGameObject;
class CGameState;

typedef struct {
    Vector2 vec2Pos;
    float fAngle;
} SpawnPoint_t;

class CServerGameState : public IGameState {
public:
    CMovementComponent* AddMovement(UnitId idUnit) {
        CMovementComponent comp;
        mapMovementIndices[idUnit] = (int)vecMovementComponents.size();
        vecMovementComponents.push_back(comp);
        return &vecMovementComponents.at(mapMovementIndices[idUnit]);
    }

    CMovementComponent* GetMovement(UnitId idUnit) {
        auto index = mapMovementIndices.at(idUnit);

        if(index == -1) {
            return nullptr;
        }

        return &vecMovementComponents.at(index) ;
    }
    
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
    std::vector<CMovementComponent> vecMovementComponents;
    std::vector<int> mapMovementIndices;

    NavigationMap* m_pNavMap;
};