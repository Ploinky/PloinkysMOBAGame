#include "Camera.h"
#include "common/pmg_physics.h"

Camera::Camera() {
    this->fov = 59;
    this->nearClip = 1.0f;
    this->farClip = 10000.0f;
    this->position = Vector3{ 2000.0f, 1500.0f,-800.0f };
    this->rotation = Vector3{ -60, 0.0f, 0.0f };
}
