#include "health-system.h"

CHealthSystem::CHealthSystem(CClientAssetManager* pAssetManager) {
    this->m_pAssetManager = pAssetManager;

    REGISTER_EVENT_HANDLER(CStatsEvent, OnDamage)
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