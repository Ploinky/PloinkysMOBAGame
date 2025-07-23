#pragma once

#include "GameObject.h"

class CHealthComponent : public IComponent{
public:
    CHealthComponent(int nMaxHealth) : m_nMaxHealth(nMaxHealth), m_nHealth(nMaxHealth) {};

    virtual void Update(CGameState* pGameState, float fDelta) override {};

    void TakeDamage(int nDamage);
    int GetCurrentHealth();
    int GetMaxHealth();

private:
    int m_nMaxHealth;
    int m_nHealth;
};