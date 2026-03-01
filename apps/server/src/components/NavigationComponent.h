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
    NavigationGridAgent* pNavGridAgent = nullptr;
    ENavigationStatus eStatus;
    float fTimeBlocked = 0.0f;
} NavigationComponent_t;