#pragma once

#include "GameObject.h"

class CHealthComponent : public IComponent{
public:
    CHealthComponent(int nMaxHealth) : nMaxHealth(nMaxHealth), nHealth(nMaxHealth) {};

    virtual void Update(CGameState* pGameState, float fDelta) override {};

    int nMaxHealth;
    int nHealth;
};