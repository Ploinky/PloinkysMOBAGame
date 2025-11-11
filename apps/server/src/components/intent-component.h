#pragma once

#include "GameObject.h"

enum class EIntentType {
    NONE,
    CAST_SPELL,
    MOVE,
    ATTACK
};

class CIntentComponent : public IComponent {
public:
    EIntentType eType;
    Vector3 vec3Target;
};