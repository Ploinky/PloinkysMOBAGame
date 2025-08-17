#include "wave-manager.h"

#include "GameState.h"
#include "GameObject.h"
#include "components/Components.h"

#define SPAWN_INTERVAL 30000

CWaveManager::CWaveManager() {
    fTimeSinceLastSpawn = 30000;
}

void CWaveManager::Update(CGameState* pGameState, float fDelta) {
    fTimeSinceLastSpawn += fDelta;

    if(fTimeSinceLastSpawn < SPAWN_INTERVAL) {
        return;
    }

    fTimeSinceLastSpawn = 0.0f;

    CGameObject* pGo = new CGameObject();
    pGo->AddComponent(new CCharacterComponent(UnitPrefab::MINION));
    pGo->AddComponent(new CTransformComponent());
    pGo->GetComponent<CTransformComponent>()->SetPosition({2000, 0, -3000});
    pGo->AddComponent(new CNetworkComponent());
    pGo->AddComponent(new CHealthComponent(50));
    pGo->AddComponent(new CTeamComponent(Team::TEAM_1));
    pGo->AddComponent(new CIntentComponent());
    pGo->AddComponent(new CAiComponent());
    pGo->AddComponent(new CNavigationComponent());
    pGo->AddComponent(new CMovementComponent());
    pGo->GetComponent<CNetworkComponent>()->SetSyncMovement(true);
    pGo->GetComponent<CAiComponent>()->eType = EAiType::MINION;
    pGo->GetComponent<CAiComponent>()->vecWaypoints.push_back({3000, 0, -2000});
    pGameState->GameObjects.emplace(pGo->GetId(), pGo);
}