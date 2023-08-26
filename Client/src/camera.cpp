#include "camera.h"
#include "pmg_physics.h"

namespace PMG {
    Camera::Camera() {
        this->fov = 60;
        this->nearClip = 0.001f;
        this->farClip = 1000.0f;
        this->position = Physics::Vector3{ 0.0f, 20.0f, -10.0f };
        this->rotation = Physics::Vector3{ 60.0f, 0.0f, 0.0f };
    }
}