#pragma once

#include "GameObject.h"

class CCharacterComponent : public IComponent{
public:
    CCharacterComponent(uint64_t prefab) : prefab(prefab) {};

    uint64_t prefab = UnitPrefab::GENERIC_EMPTY;
};