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

    CGameObject* pGo = new CGameObject();
    UnitId id = pGo->GetId();
    pGameState->AddMovement(id);
    pGameState->AddCharacter(id, CCharacterComponent(UnitPrefab::MINION));
    pGameState->AddTransform(id);
    pGameState->GetTransform(id)->SetPosition({2000, 0, -3000});
    pGameState->AddNetwork(id, CNetworkComponent(true));
    pGameState->AddHealth(id, CHealthComponent(50));
    pGameState->AddTeam(id, CTeamComponent(Team::TEAM_1));
    pGameState->AddIntent(id);
    pGameState->AddAi(id);
    pGameState->AddNavigation(id);
    pGameState->GetAi(id)->eType = EAiType::MINION;
    pGameState->GetAi(id)->vecWaypoints.push_back({3000, 0, -2000});
    pGameState->GameObjects.emplace(pGo->GetId(), pGo);
}