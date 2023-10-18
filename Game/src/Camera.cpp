#include "Camera.h"
#include "pmg_physics.h"

namespace PMG {
    Camera::Camera() {
        this->fov = 59;
        this->nearClip = 0.001f;
        this->farClip = 1000.0f;
        this->position = Physics::Vector3{ 0.0f, 20.0f, -8 };
        this->rotation = Physics::Vector3{ 60, 0.0f, 0.0f };
    }
}