#pragma once

#include "GameObject.h"
#include "common/PMG_Common.h"
#include "common/pmg_types.h"

typedef struct TransformComponent_t {
    public:
    Vector3 GetPosition() const;
    Vector3 GetRotation() const;
    
    void SetPosition(Vector3 vec3NewRotation);
    void SetRotation(Vector3 vec3NewRotation);

    private:
    Vector3 m_vec3Position;
    Vector3 m_vec3Rotation;
} TransformComponent_t;