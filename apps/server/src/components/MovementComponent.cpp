#include "MovementComponent.h"
#include "TransformComponent.h"

Vector3 CMovementComponent::GetTarget() const {
    return m_vec3Target;
}

void CMovementComponent::SetTarget(Vector3 vec3Target) {
    m_vec3Target = vec3Target;
}

void CMovementComponent::ClearTarget() {
    if(CTransformComponent* pTransform = m_pGameObject->GetComponent<CTransformComponent>()) {
        SetTarget(pTransform->GetPosition());
    }
}