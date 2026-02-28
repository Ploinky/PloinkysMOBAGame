#include "wave-manager.h"

#include "game/server-game-state.h"
#include "GameObject.h"
#include "components/Components.h"
#include <game/server-game-state.h>

#define SPAWN_INTERVAL 30000

CWaveManager::CWaveManager() {
    fTimeSinceLastSpawn = 30000;
}

void CWaveManager::Update(CServerGameState* pGameState, float fDelta) {
    fTimeSinceLastSpawn += fDelta;

    if(fTimeSinceLastSpawn < SPAWN_INTERVAL) {
        return;
    }

    fTimeSinceLastSpawn = 0.0f;

    UnitId id = pGameState->SpawnUnit({}, "minion"); // TODO fix
    pGameState->GetTransform(id)->SetPosition({2000, 0, -3000});

    pGameState->AddTeam(id, TeamComponent_t(Team::TEAM_1));
    pGameState->AddAi(id);
    pGameState->GetAi(id)->eType = EAiType::MINION;
    pGameState->GetAi(id)->vecWaypoints.push_back({3000, 0, -2000});
}