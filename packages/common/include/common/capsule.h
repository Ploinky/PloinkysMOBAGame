#pragma once

#include "vector3.h"

typedef struct Capsule_s {
    Vector3 vec3Start;
    Vector3 vec3End;
    float fRadius;
} Capsule_t;