#include <GameObject.h>

CMovementComponent::CMovementComponent() {
	m_vec3Target = Vector3::ZERO;
	m_bIsMoving = false;
}
Vector3 CMovementComponent::GetTarget() {
	return m_vec3Target;
}

void CMovementComponent::SetTarget(Vector3 vec3NewTarget) {
	m_vec3Target = vec3NewTarget;
	m_bIsMoving = true;
}

void CMovementComponent::ClearTarget() {
	m_vec3Target = Vector3::ZERO;
	m_bIsMoving = false;
}

bool CMovementComponent::IsMoving() {
	return m_bIsMoving;
}