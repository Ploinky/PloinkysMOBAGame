#pragma once

#include "common/pmg_types.h"

enum class EIntentType {
    NONE,
    CAST_SPELL,
    MOVE,
    ATTACK
};

typedef struct IntentComponent_t {
    UnitId idUnit;
    EIntentType eType;
    Vector3 vec3Target;
} IntentComponent_t;