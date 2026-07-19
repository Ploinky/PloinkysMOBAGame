#include "movement-system.h"

#include "game/server-game-state.h"
#include "GameObject.h"
#include "components/Components.h"
#include "events.h"

CMovementSystem::CMovementSystem() {
}

void CMovementSystem::Update(CServerGameState* pGameState, float fDelta) {
    for(auto& [id, move] : pGameState->GetAllMovement()) {
        UpdateEntity(pGameState, fDelta, move, id);
    }
}

void CMovementSystem::UpdateEntity(CServerGameState* pGameState, float fDelta, MovementComponent_t& move, UnitId id) const {
    HealthComponent_t* pHealthComp = pGameState->GetHealth(id);
    if(pHealthComp != nullptr && pHealthComp->bIsDead) {
        return;
    }

    TransformComponent_t* pTransform = pGameState->GetTransform(id);
    if(!pTransform) {
        // TODO we can't move
        return;
    }

    Vector3 vec3OldPosition = pTransform->GetPosition();

    if(CompareFloat((move.vec3Target - vec3OldPosition).Length(), 0 )) {
        // TODO we're at our target
        move.vec3Target = pTransform->GetPosition();
        return;
    }

    // For navigating units, use the nav map...
    if(NavigationComponent_t* pNavComp = pGameState->GetNavigation(id)) {
        StepResult_t step = pGameState->m_pNavMap->Step(pNavComp->pNavGridAgent, {vec3OldPosition.x, vec3OldPosition.z}, move.fSpeed * (fDelta / 1000.0f));
        Vector2 vec2Step = step.vec2Pos;

        
        // TODO only check units in my vicinity
        // TODO improve collisioning code
        if (step.bBlocked) {
            if(pNavComp->eStatus != ENavigationStatus::BLOCKED) {
                pNavComp->eStatus = ENavigationStatus::BLOCKED;
                pNavComp->fTimeBlocked = 0.0f;
            }
            Logger::FormatMsg("Unit %d unable to move to %f, %f - blocked! Stopping!", id, vec2Step.x, vec2Step.y);
            return;
        }
        pTransform->SetPosition({vec2Step.x, vec3OldPosition.y, vec2Step.y});
    } else {
        Vector3 vec3Move = move.vec3Target - vec3OldPosition;

        if(vec3Move.Length() >= (move.fSpeed * (fDelta / 1000.0f))) {
            vec3Move = vec3Move.ScaleToLength(move.fSpeed * (fDelta / 1000.0f));
        }
        pTransform->SetPosition(vec3OldPosition + vec3Move);
    }
    
    pTransform->SetRotation({0, CalculateAngle({vec3OldPosition.x, vec3OldPosition.z}, {move.vec3Target.x, move.vec3Target.z}), 0});

    pGameState->EmitEvent(new CMoveEvent(id, pTransform->GetPosition(), pTransform->GetRotation().y));
}

void CMovementSystem::OnDeath(CServerGameState* pGameState, CDeathEvent* pDeathEvt) {
    MovementComponent_t* pMoveComp = pGameState->GetMovement(pDeathEvt->idTarget);
    TransformComponent_t* pTransformComp = pGameState->GetTransform(pDeathEvt->idTarget);
    if(pMoveComp == nullptr) {
        return;
    }

    pMoveComp->vec3Target = pTransformComp->GetPosition();
}