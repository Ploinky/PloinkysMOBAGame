#pragma once

#include <DirectXMath.h>
#include "pmg_physics.h"

namespace PMG {
    class Camera {
        public:
            Physics::Vector3 position;
            Physics::Vector3 rotation;
            float fov;
            float nearClip;
            float farClip;

            Camera();
    };
}