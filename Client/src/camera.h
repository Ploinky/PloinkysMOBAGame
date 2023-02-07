#pragma once

#include <DirectXMath.h>
#include "physics.h"

namespace PMG {
    class Camera {
        public:
            vec3_t position;
            vec3_t rotation;
            float fov;
            float nearClip;
            float farClip;

            Camera();
    };
}