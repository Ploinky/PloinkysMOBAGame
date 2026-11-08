#include "respawn-system.h"

CRespawnSystem::CRespawnSystem() {

}

void CRespawnSystem::Update(CServerGameState* pGameState, float fDelta) {
    /*

    for (auto& [id, respawn] : pGameState->GetAllRespawn()) {
        if (respawn.fRespawnCountdown <= 0.0f) {
            continue;
        }

        respawn.fRespawnCountdown -= fDelta;

        if (respawn.fRespawnCountdown <= 0.0f) {
            respawn.fRespawnCountdown = 0.0f;

            pGameState->EmitEvent(CRespawnEvent(id));

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
            */

}

void CRespawnSystem::OnRoundReset(CServerGameState* pGameState, const CRoundResetEvent& evt) {
    for (auto& [id, respawn] : pGameState->GetAllRespawn()) {
        if (pGameState->GetHealth(id)->bIsDead) {
            pGameState->EmitEvent(CRespawnEvent(id));
        }
    }
}

void CRespawnSystem::OnEntityDied(CServerGameState* pGameState, const CDeathEvent& evt) {
    if (RespawnComponent_t* pRespawn = pGameState->GetRespawn(evt.idTarget)) {
        pRespawn->fRespawnCountdown = 3000.0f;
    }
}