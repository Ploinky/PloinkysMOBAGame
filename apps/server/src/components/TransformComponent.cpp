#include "TransformComponent.h"

Vector3 TransformComponent_t::GetPosition() const {
    return m_vec3Position;
}

Vector3 TransformComponent_t::GetRotation() const {
    return m_vec3Rotation;
}

void TransformComponent_t::SetPosition(Vector3 vec3NewPosition) {
    m_vec3Position = vec3NewPosition;
}

void TransformComponent_t::SetRotation(Vector3 vec3NewRotation) {
    m_vec3Rotation = vec3NewRotation;
}