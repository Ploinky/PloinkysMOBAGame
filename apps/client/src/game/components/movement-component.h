#pragma once

#include "common/PMG_Common.h"

typedef struct {
    Vector3 vec3Target;
    bool bIsMoving;
    float fSpeed;
} MovementComponent_t;
