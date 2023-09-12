#include "camera.h"
#include "pmg_physics.h"

namespace PMG {
    Camera::Camera() {
        this->fov = 30;
        this->nearClip = 0.001f;
        this->farClip = 1000.0f;
        this->position = Physics::Vector3{ 0.0f, 0.0f, 0 };
        this->rotation = Physics::Vector3{ 0, 0.0f, 0.0f };
    }
}