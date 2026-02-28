#include "health-system.h"

CHealthSystem::CHealthSystem(CClientAssetManager* pAssetManager) {
    this->m_pAssetManager = pAssetManager;
}

void CHealthSystem::Update(CClientGameState* pGameState, float fDelta) {
    // nothing to do
}

void CHealthSystem::OnDamage(CClientGameState* pGameState, CStatsEvent* pEvent) {
    HealthComponent_t* pHealthComp = pGameState->GetHealth(pEvent->idUnit);
    
    if(pHealthComp) {
        pHealthComp->nHealth = pEvent->nHealth;
        pHealthComp->nMaxHealth = pEvent->nMaxHealth;
    }
}

void CHealthSystem::OnDeath(CClientGameState* pGameState, CEntityDeathEvent* pEvent) {
    if(HealthComponent_t* pHealthComp = pGameState->GetHealth(pEvent->idUnit)) {
        pHealthComp->bIsDead = true;
        pHealthComp->nHealth = 0;
    }
}