#include "movement-system.h"

#include "game/server-game-state.h"
#include "GameObject.h"
#include "components/Components.h"
#include "events.h"

CMovementSystem::CMovementSystem() {
    REGISTER_EVENT_HANDLER(CDeathEvent, OnDeath);
}

void CMovementSystem::Update(CServerGameState* pGameState, float fDelta) {
    for(CMovementComponent& move : pGameState->GetAllMovement()) {

        CHealthComponent* pHealthComp = pGameState->GetHealth(move.idUnit);
        if(pHealthComp != nullptr && pHealthComp->bIsDead) {
            continue;
        }

        CTransformComponent* pTransform = pGameState->GetTransform(move.idUnit);
        if(!pTransform) {
            // TODO we can't move
            continue;
        }

        Vector3 vec3OldPosition = pTransform->GetPosition();

        if(CompareFloat((move.vec3Target - vec3OldPosition).Length(), 0 )) {
            // TODO we're at our target
            move.vec3Target = pTransform->GetPosition();
            continue;
        }

        // For navigating units, use the nav map...
        if(CNavigationComponent* pNavComp = pGameState->GetNavigation(move.idUnit)) {
            Vector2 vec2Step = pGameState->m_pNavMap->Step(pNavComp->pNavGridAgent, {vec3OldPosition.x, vec3OldPosition.z}, move.fSpeed * (fDelta / 1000.0f));

            pTransform->SetPosition({vec2Step.x, vec3OldPosition.y, vec2Step.y});
        } else {
            Vector3 vec3Move = move.vec3Target - vec3OldPosition;

            if(vec3Move.Length() >= (move.fSpeed * (fDelta / 1000.0f))) {
                vec3Move = vec3Move.ScaleToLength(move.fSpeed * (fDelta / 1000.0f));
            }
            pTransform->SetPosition(vec3OldPosition + vec3Move);
        }
        
        pTransform->SetRotation({0, CalculateAngle({vec3OldPosition.x, vec3OldPosition.z}, {move.vec3Target.x, move.vec3Target.z}), 0});

        pGameState->VecEvent.emplace(new CMoveEvent(move.idUnit, pTransform->GetPosition(), pTransform->GetRotation().y));
    }
}

void CMovementSystem::OnDeath(CServerGameState* pGameState, CDeathEvent* pDeathEvt) {
    CMovementComponent* pMoveComp = pGameState->GetMovement(pDeathEvt->idTarget);
    CTransformComponent* pTransformComp = pGameState->GetTransform(pDeathEvt->idTarget);
    if(pMoveComp == nullptr) {
        return;
    }

    pMoveComp->vec3Target = pTransformComp->GetPosition();
}