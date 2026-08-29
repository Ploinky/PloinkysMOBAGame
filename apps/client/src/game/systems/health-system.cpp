#include "health-system.h"

CHealthSystem::CHealthSystem(CClientAssetManager* pAssetManager) {
    this->m_pAssetManager = pAssetManager;
}

void CHealthSystem::Update(CClientGameState* pGameState, float fDelta) {
    // nothing to do
}

void CHealthSystem::OnDamage(CClientGameState* pGameState, const CStatsEvent& evt) {
    HealthComponent_t* pHealthComp = pGameState->GetHealth(evt.idUnit);
    
    if(pHealthComp) {
        pHealthComp->nHealth = evt.nHealth;
        pHealthComp->nMaxHealth = evt.nMaxHealth;
    }
}

void CHealthSystem::OnDeath(CClientGameState* pGameState, const CEntityDeathEvent& evt) {
    if(HealthComponent_t* pHealthComp = pGameState->GetHealth(evt.idUnit)) {
        pHealthComp->bIsDead = true;
        pHealthComp->nHealth = 0;
    }
}