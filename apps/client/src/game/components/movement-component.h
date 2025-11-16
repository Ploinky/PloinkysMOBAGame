#pragma once

#include "common/PMG_Common.h"

typedef struct {
    UnitId idUnit;
    Vector3 vec3Target;
    bool bIsMoving;
} MovementComponent_t;
