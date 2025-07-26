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