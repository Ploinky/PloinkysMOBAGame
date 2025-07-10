#include "MovementComponent.h"
#include "TransformComponent.h"
#include "EventManager.h"

Vector3 CMovementComponent::GetTarget() const {
    return m_vec3Target;
}

void CMovementComponent::SetTarget(Vector3 vec3Target) {
    m_vec3Target = vec3Target;

    MoveIntentionData_t data {m_pGameObject->GetId(), vec3Target.x, vec3Target.z};
    CEventManager::Emit(EEventType::MOVE_INTENTION, &data);
}

void CMovementComponent::ClearTarget() {
    if(CTransformComponent* pTransform = m_pGameObject->GetComponent<CTransformComponent>()) {
        SetTarget(pTransform->GetPosition());
    }
}

void CMovementComponent::Update(CGameState* pGameState, float fDelta) {
    CTransformComponent* pTransform = m_pGameObject->GetComponent<CTransformComponent>();
    if(!pTransform) {
        // TODO we can't move
        return;
    }
    Vector3 vec3OldPosition = pTransform->GetPosition();

    if(CompareFloat((GetTarget() - vec3OldPosition).Length(), 0 )) {
        // TODO we're at our target
        return;
    }

    Vector3 vec3Move = GetTarget() - vec3OldPosition;

    if(vec3Move.Length() < 10) {
        pTransform->SetPosition(GetTarget());
        pTransform->SetRotation({0, CalculateAngle({vec3OldPosition.x, vec3OldPosition.z}, {GetTarget().x, GetTarget().z}), 0});
        return;
    }

    vec3Move = vec3Move.ScaleToLength(10);

    pTransform->SetPosition(vec3OldPosition + vec3Move);
    pTransform->SetRotation({0, CalculateAngle({vec3OldPosition.x, vec3OldPosition.z}, {GetTarget().x, GetTarget().z}), 0});
}