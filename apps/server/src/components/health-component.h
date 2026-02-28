#pragma once

#include "GameObject.h"
#include "common/pmg_types.h"

typedef struct HealthComponent_t {
    HealthComponent_t() : HealthComponent_t(100) {};
    HealthComponent_t(int nMaxHealth) : nMaxHealth(nMaxHealth), nHealth(nMaxHealth) {};
    
    UnitId idUnit;
    int nMaxHealth = 100;
    int nHealth = 100;
    bool bIsDead = false;
    float fTimeSinceDeath = 0.0f;
} HealthComponent_t;