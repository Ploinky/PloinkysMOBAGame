#pragma once

#include "common/pmg_types.h"

typedef struct MissileComponent_s {
    Vector3 vec3Target;
    float fSpeed;
    UnitId idOwner;
} MissileComponent_t;