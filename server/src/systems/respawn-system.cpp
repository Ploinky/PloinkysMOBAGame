#include "respawn-system.h"

CRespawnSystem::CRespawnSystem() {

}

void CRespawnSystem::Update(CGameState* pGameState, float fDelta) {
    for(std::pair<UnitId, CGameObject*> goPair : pGameState->GameObjects) {
        CGameObject* pGameObject = goPair.second;

        CHealthComponent* pHealthComp = pGameObject->GetComponent<CHealthComponent>();

        if(pHealthComp == nullptr) {
            continue;
        }

        if(pHealthComp->bIsDead) {
            pHealthComp->fTimeSinceDeath += fDelta;

            if(pHealthComp->fTimeSinceDeath >= 3000.0f) {
                pHealthComp->nHealth = pHealthComp->nMaxHealth;
                pHealthComp->bIsDead = false;
                pHealthComp->fTimeSinceDeath = 0.0f;

                CTransformComponent* pTransform = pGameObject->GetComponent<CTransformComponent>();
                CTeamComponent* pTeam = pGameObject->GetComponent<CTeamComponent>();

                if(pTeam != nullptr && pTransform != nullptr) {
                    SpawnPoint_t spawn = pGameState->mapTeamSpawnPoints[pTeam->eTeam].at(0);
                    pTransform->SetPosition({spawn.vec2Pos.x, 0, spawn.vec2Pos.y});
                    pTransform->SetRotation({0, spawn.fAngle, 0});

                    if(pGameObject->GetComponent<CMovementComponent>()) {
                        pGameObject->GetComponent<CMovementComponent>()->ClearTarget();
                    }

                    pGameState->VecEvent.emplace(new CMoveEvent(pGameObject->GetId(), pTransform->GetPosition(), pTransform->GetRotation().y));
                }


                pGameState->VecEvent.emplace(new CRespawnEvent(pGameObject->GetId()));
            }
        }
    }
}