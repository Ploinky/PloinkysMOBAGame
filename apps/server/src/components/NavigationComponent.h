#pragma once

#include "GameObject.h"

class CNavigationComponent : public IComponent {
public:
    Vector3 vec3Destination;
    NavigationGridAgent* pNavGridAgent = nullptr;
    bool bIsNavigating;
};