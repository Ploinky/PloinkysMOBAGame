#pragma once

#include "GameObject.h"

class CHealthComponent : public IComponent{
public:
    CHealthComponent() : CHealthComponent(100) {};
    CHealthComponent(int nMaxHealth) : nMaxHealth(nMaxHealth), nHealth(nMaxHealth) {};

    int nMaxHealth = 100;
    int nHealth = 100;
    bool bIsDead = false;
    float fTimeSinceDeath = 0.0f;
};