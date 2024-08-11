#include "Camera.h"
#include "common/pmg_physics.h"

namespace PMG {
    Camera::Camera() {
        this->fov = 59;
        this->nearClip = 0.1f;
        this->farClip = 100000.0f;
        this->position = Physics::Vector3{ 2000.0f, 1500.0f, -800.0f };
        this->rotation = Physics::Vector3{ -60, 0.0f, 0.0f };
    }
}