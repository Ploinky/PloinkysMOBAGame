#pragma once

#include "common/PMG_Common.h"
#include "Components.h"

class CMovementComponent : public IComponent {
public:
    Vector3 vec3Target;
    float fSpeed;
};