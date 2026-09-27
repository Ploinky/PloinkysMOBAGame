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
    Vector3 vec3Destination;
    HGridBlocker hBlocker = INVALID_HANDLE;
    std::vector<PathSegment_t> path;
    ENavigationStatus eStatus;
    float fTimeBlocked = 0.0f;
    int nCollisionRadius;
} NavigationComponent_t;