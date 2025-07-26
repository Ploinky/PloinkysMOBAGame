#pragma once

#include "GameObject.h"
#include "Common/PMG_Common.h"

class CTransformComponent : public IComponent {
public:
    Vector3 GetPosition() const;
    Vector3 GetRotation() const;

    void SetPosition(Vector3 vec3NewRotation);
    void SetRotation(Vector3 vec3NewRotation);

    private:
    Vector3 m_vec3Position;
    Vector3 m_vec3Rotation;
};