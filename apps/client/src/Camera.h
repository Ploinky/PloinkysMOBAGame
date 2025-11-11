#pragma once

#include <common/pmg_physics.h>

class Camera {
    public:
        Vector3 position;
        Vector3 rotation;
        float fov;
        float nearClip;
        float farClip;

        Camera();
};
