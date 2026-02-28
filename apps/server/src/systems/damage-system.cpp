#include "damage-system.h"

#include "game/server-game-state.h"
#include "events/damage-event.h"
#include "events/heal-event.h"
#include "events.h"
#include "GameObject.h"
#include "components/Components.h"


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
    
                pGameState->EmitEvent(new CDeathEvent(id));
            }

            if(health.nHealth > health.nMaxHealth) {
                health.nHealth = health.nMaxHealth;
            }
        }

    }
}

void CDamageSystem::OnUnitDamaged(CServerGameState* pGameState, CDamageEvent* dmgEvt) {
    HealthComponent_t* pHealthComponent = pGameState->GetHealth(dmgEvt->m_idTarget);

    if(pHealthComponent == nullptr) {
        Logger::FormatErr("Invalid damage event: target unit (%d) does nmot have a health component", dmgEvt->m_idTarget);
        return;
    }

    if(pHealthComponent->bIsDead) {
        return;
    }

    pHealthComponent->nHealth -= dmgEvt->m_nDamage;
}

void CDamageSystem::OnUnitHealed(CServerGameState* pGameState, CHealEvent* healEvt) {
    HealthComponent_t* pHealthComponent = pGameState->GetHealth(healEvt->m_idTarget);
    pHealthComponent->nHealth += healEvt->m_nHeal;
}