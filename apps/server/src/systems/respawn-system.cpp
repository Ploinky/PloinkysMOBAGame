#include "respawn-system.h"

CRespawnSystem::CRespawnSystem() {

}

void CRespawnSystem::Update(CServerGameState* pGameState, float fDelta) {
    for(auto& [id, health] : pGameState->GetAllHealth()) {
        if(health.bIsDead) {
            health.fTimeSinceDeath += fDelta;

            if(health.fTimeSinceDeath >= 3000.0f) {
                health.nHealth = health.nMaxHealth;
                health.bIsDead = false;
                health.fTimeSinceDeath = 0.0f;

                TransformComponent_t* pTransform = pGameState->GetTransform(id);
                TeamComponent_t* pTeam = pGameState->GetTeam(id);

                if(pTeam != nullptr && pTransform != nullptr) {
                    SpawnPoint_t spawn = pGameState->mapTeamSpawnPoints[pTeam->eTeam].at(0);
                    pTransform->SetPosition({spawn.vec2Pos.x, 0, spawn.vec2Pos.y});
                    pTransform->SetRotation({0, spawn.fAngle, 0});

                    if(pGameState->GetMovement(id)) {
                        if(TransformComponent_t* pTransform =pGameState->GetTransform(id)) {
                            pGameState->GetMovement(id)->vec3Target = pTransform->GetPosition();

                        }
                    }

                    pGameState->EmitEvent(new CMoveEvent(id, pTransform->GetPosition(), pTransform->GetRotation().y));
                }


                pGameState->EmitEvent(new CRespawnEvent(id));
            }
        }
    }
}