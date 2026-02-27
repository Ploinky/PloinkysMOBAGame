#pragma once

#include "GameObject.h"
#include <deque>

enum class EAiType {
    MINION
};

class CAiComponent : public IComponent {
public:
    CAiComponent() {};
    
    EAiType eType;
    // minion
    std::deque<Vector3> vecWaypoints;
};