#include "TransformComponent.h"

Vector3 CTransformComponent::GetPosition() const {
    return m_vec3Position;
}

Vector3 CTransformComponent::GetRotation() const {
    return m_vec3Rotation;
}

void CTransformComponent::SetPosition(Vector3 vec3NewPosition) {
    m_vec3Position = vec3NewPosition;
}

void CTransformComponent::SetRotation(Vector3 vec3NewRotation) {
    m_vec3Rotation = vec3NewRotation;
}

void CTransformComponent::Update(CGameState* pGameState, float fDelta) {
}