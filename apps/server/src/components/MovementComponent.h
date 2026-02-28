#pragma once

#include "common/PMG_Common.h"
#include "Components.h"
#include "common/pmg_types.h"

typedef struct MovementComponent_t {
    UnitId idUnit;
    Vector3 vec3Target;
    float fSpeed;
} MovementComponent_t;