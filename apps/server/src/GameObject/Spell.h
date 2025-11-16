#pragma once

#include "GameObject.h"

class CSpellCastContext {
public:
    UnitId idCaster = UNIT_ID_NONE;
    int nSpellIndex = -1;
    UnitId idTarget = UNIT_ID_NONE;
    Vector2 vec2Target = Vector2(0, 0);
    Vector2 vec2Dir = Vector2(0, 0);
};