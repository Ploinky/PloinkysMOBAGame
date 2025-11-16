#pragma once

#include <string>

#include "GameObject.h"

class CCharacterComponent : public IComponent {
public:
    CCharacterComponent() : prefab("") {};
    CCharacterComponent(std::string prefab) : prefab(prefab) {};

    std::string prefab = "";
};