#include "missile-system.h"

CMissileSystem::CMissileSystem() {

}

void CMissileSystem::Update(CServerGameState* pGameState, float fDelta) {
    if (pGameState->eRoundState == ERoundState::PRE_ROUND
        || pGameState->eRoundState == ERoundState::POST_ROUND
        || pGameState->eRoundState == ERoundState::POST_GAME
    ) {
        return;
    }

    for (auto [id, missile] : pGameState->GetAllMissile()) {
        MovementComponent_t* pMove = pGameState->GetMovement(id);
        TransformComponent_t* pTransform = pGameState->GetTransform(id);
        TeamComponent_t* pTeam = pGameState->GetTeam(id);

        if (pMove) {
            pMove->vec3Target = missile.vec3Target;
        }

        // remove if target reached
        if (pMove && pTransform && pTeam) {
            std::vector<UnitId> unitsColliding = pGameState->GetUnitsInAra(Vector2(pTransform->GetPosition().x, pTransform->GetPosition().z), 80);

            for (UnitId idOther : unitsColliding) {
                TeamComponent_t* pOtherTeam = pGameState->GetTeam(idOther);
                if (idOther == id || pOtherTeam == nullptr) {
                    continue;
                }
                if (pTeam->eTeam == pOtherTeam->eTeam) {
                    // TODO pick targets better
                    continue;
                }

                // TODO who gets hit first?
                // hit a unit?
                pGameState->EmitEvent(CDamageEvent(missile.idOwner, idOther, 1000));
                pGameState->EmitEvent(CRemoveEvent(id));
                // TODO dangerous code (blocking out later if blocks) and what if I can hit multiple?
                return;
            }
        }
            
        if (pMove && pTransform) {
            if (pTransform->GetPosition() == missile.vec3Target) {
                pGameState->EmitEvent(CRemoveEvent(id));
            }
        }
    }
}