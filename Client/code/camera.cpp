#include "camera.hpp"

namespace PMG {
    Camera::Camera() {
        this->fov = 60;
        this->nearClip = 0.001f;
        this->farClip = 1000.0f;
        this->position = DirectX::XMFLOAT3(0, 20, -5);
        this->rotation = DirectX::XMFLOAT3(60, 0, 0);
    }
}