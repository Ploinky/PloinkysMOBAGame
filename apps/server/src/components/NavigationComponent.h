#pragma once

#include "common/pmg_types.h"

enum class ENavigationStatus {
    IDLE,
    PLANNING,
    PATHING,
    BLOCKED,
    ARRIVED // navigation system thinks entity is as close as it will get
};

typedef struct NavigationComponent_t {
    UnitId idUnit;
    Vector3 vec3Destination;
    NavigationGridAgent* pNavGridAgent = nullptr;
    ENavigationStatus eStatus;
} NavigationComponent_t;