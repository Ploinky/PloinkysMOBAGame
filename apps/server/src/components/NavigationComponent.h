#pragma once

#include "GameObject.h"

class CNavigationComponent : public IComponent {
public:
    Vector3 vec3Destination;
    NavigationGridAgent* pNavGridAgent = new NavigationGridAgent();
    bool bIsNavigating;
};