#pragma once

#include <DirectXMath.h>
#include <Common/pmg_physics.h>

class Camera {
    public:
        Vector3 position;
        Vector3 rotation;
        float fov;
        float nearClip;
        float farClip;

        Camera();
};
