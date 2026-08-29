#include "damage-system.h"

#include "../game/server-game-state.h"
#include "../events.h"
#include "../GameObject.h"
#include "../components/Components.h"


CDamageSystem::CDamageSystem() {
}

void CDamageSystem::Update(CServerGameState* pGameState, float fDelta) { 
}

void CDamageSystem::Finalize(CServerGameState* pGameState) {
    for(auto& [id, health] : pGameState->GetAllHealth()) {
        if(!health.bIsDead) {
            if(health.nHealth <= 0) {
                health.nHealth = 0;
                health.bIsDead = true;
                health.fTimeSinceDeath = 0.0f;
    
                pGameState->EmitEvent(CDeathEvent(id));
            }

            if(health.nHealth > health.nMaxHealth) {
                health.nHealth = health.nMaxHealth;
            }
        }

    }
}

void CDamageSystem::OnUnitDamaged(CServerGameState* pGameState, const CDamageEvent& evt) {
    HealthComponent_t* pHealthComponent = pGameState->GetHealth(evt.m_idTarget);

    if(pHealthComponent == nullptr) {
        Logger::FormatErr("Invalid damage event: target unit (%d) does nmot have a health component", evt.m_idTarget);
        return;
    }

    if(pHealthComponent->bIsDead) {
        return;
    }

    pHealthComponent->nHealth -= evt.m_nDamage;
}

void CDamageSystem::OnUnitHealed(CServerGameState* pGameState, const CHealEvent& evt) {
    HealthComponent_t* pHealthComponent = pGameState->GetHealth(evt.m_idTarget);
    pHealthComponent->nHealth += evt.m_nHeal;
}

void CDamageSystem::OnUnitRespawned(CServerGameState* pGameState, const CRespawnEvent& evt) {
    if (HealthComponent_t* pHealth = pGameState->GetHealth(evt.idTarget)) {
        pHealth->nHealth = pHealth->nMaxHealth;
        pHealth->bIsDead = false;
        pHealth->fTimeSinceDeath = 0.0f;
    }
}