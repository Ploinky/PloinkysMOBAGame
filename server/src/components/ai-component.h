#pragma once

#include "GameObject.h"
#include <vector>

enum class EAiType {
    MINION
};

class CAiComponent : public IComponent {
public:
    CAiComponent() {};
    
    EAiType eType;
    // minion
    std::vector<Vector3> vecWaypoints;
};