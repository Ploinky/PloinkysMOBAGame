#include "camera.h"

namespace PMG {
    Camera::Camera() {
        this->fov = 60;
        this->nearClip = 0.001f;
        this->farClip = 1000.0f;
        this->position = vec3_t { 0.0f, 20.0f, -5.0f };
        this->rotation = vec3_t { 60.0f, 0.0f, 0.0f };
    }
}