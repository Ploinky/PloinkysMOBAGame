#pragma once

#include "GameObject.h"

enum class ENavigationStatus {
    IDLE,
    PLANNING,
    PATHING,
    BLOCKED,
    ARRIVED // navigation system thinks entity is as close as it will get
};

class CNavigationComponent : public IComponent {
public:
    Vector3 vec3Destination;
    NavigationGridAgent* pNavGridAgent = nullptr;
    ENavigationStatus eStatus;
};