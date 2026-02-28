#pragma once

#include <deque>
#include "common/pmg_types.h"
#include "common/vector3.h"


enum class EAiType {
    MINION
};

typedef struct {
    UnitId idUnit;
    EAiType eType;
    // minion
    std::deque<Vector3> vecWaypoints;
} AiComponent_t;