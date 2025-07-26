#pragma once

#include "GameObject.h"

class CHealthComponent : public IComponent{
public:
    CHealthComponent(int nMaxHealth) : nMaxHealth(nMaxHealth), nHealth(nMaxHealth) {};

    int nMaxHealth;
    int nHealth;
};