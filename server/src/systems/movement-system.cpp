#include "movement-system.h"

#include "GameState.h"
#include "GameObject.h"
#include "components/Components.h"
#include "events.h"

CMovementSystem::CMovementSystem() {
    REGISTER_EVENT_HANDLER(CDeathEvent, OnDeath);
}

void CMovementSystem::Update(CGameState* pGameState, float fDelta) {
    for(std::pair<UnitId, CGameObject*> goPair : pGameState->GameObjects) {
        CGameObject* pGameObject = goPair.second;

        CHealthComponent* pHealthComp = pGameObject->GetComponent<CHealthComponent>();
        if(pHealthComp != nullptr && pHealthComp->bIsDead) {
            continue;
        }

        CMovementComponent* pMoveComp = pGameObject->GetComponent<CMovementComponent>();

        if(pMoveComp == nullptr) {
            continue;
        }

        CTransformComponent* pTransform = pGameObject->GetComponent<CTransformComponent>();
        if(!pTransform) {
            // TODO we can't move
            continue;
        }

        Vector3 vec3OldPosition = pTransform->GetPosition();

        if(CompareFloat((pMoveComp->GetTarget() - vec3OldPosition).Length(), 0 )) {
            // TODO we're at our target
            pMoveComp->ClearTarget();
            return;
        }

        Vector3 vec3Move = pMoveComp->GetTarget() - vec3OldPosition;

        if(vec3Move.Length() < 10) {
            pTransform->SetPosition(pMoveComp->GetTarget());
            pTransform->SetRotation({0, CalculateAngle({vec3OldPosition.x, vec3OldPosition.z}, {pMoveComp->GetTarget().x, pMoveComp->GetTarget().z}), 0});
            return;
        }

        vec3Move = vec3Move.ScaleToLength(10);

        pTransform->SetPosition(vec3OldPosition + vec3Move);
        pTransform->SetRotation({0, CalculateAngle({vec3OldPosition.x, vec3OldPosition.z}, {pMoveComp->GetTarget().x, pMoveComp->GetTarget().z}), 0});

        pGameState->VecEvent.emplace(new CMoveEvent(goPair.first, pTransform->GetPosition(), pTransform->GetRotation().y));
    }
}

void CMovementSystem::OnDeath(CGameState* pGameState, CDeathEvent* pDeathEvt) {
    CGameObject* pGameObject = pGameState->FindGameObjectById(pDeathEvt->idTarget);

    CMovementComponent* pMoveComp = pGameObject->GetComponent<CMovementComponent>();

    if(pMoveComp == nullptr) {
        return;
    }

    pMoveComp->ClearTarget();
}