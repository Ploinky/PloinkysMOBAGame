#include "damage-system.h"

#include "game/server-game-state.h"
#include "events/damage-event.h"
#include "events/heal-event.h"
#include "events.h"
#include "GameObject.h"
#include "components/Components.h"


CDamageSystem::CDamageSystem() {
    REGISTER_EVENT_HANDLER(CDamageEvent, OnUnitDamaged);
    REGISTER_EVENT_HANDLER(CHealEvent, OnUnitHealed);
}

void CDamageSystem::Update(CServerGameState* pGameState, float fDelta) { 
    for(std::pair<UnitId, CGameObject*> goPair : pGameState->GameObjects) {
    }
}

void CDamageSystem::Finalize(CServerGameState* pGameState) {
    for(std::pair<UnitId, CGameObject*> goPair : pGameState->GameObjects) {
        CGameObject* pGameObject = goPair.second;

        CHealthComponent* pHealthComp = pGameObject->GetComponent<CHealthComponent>();

        if(pHealthComp == nullptr) {
            continue;
        }

        if(!pHealthComp->bIsDead) {
            if(pHealthComp->nHealth <= 0) {
                pHealthComp->nHealth = 0;
                pHealthComp->bIsDead = true;
                pHealthComp->fTimeSinceDeath = 0.0f;
    
                pGameState->VecEvent.emplace(new CDeathEvent(pGameObject->GetId()));
            }

            if(pHealthComp->nHealth > pHealthComp->nMaxHealth) {
                pHealthComp->nHealth = pHealthComp->nMaxHealth;
            }
        }

    }
}

void CDamageSystem::OnUnitDamaged(CServerGameState* pGameState, CDamageEvent* dmgEvt) {
    CGameObject* pSource = pGameState->FindGameObjectById(dmgEvt->m_idSource);
    CGameObject* pTarget = pGameState->FindGameObjectById(dmgEvt->m_idTarget);

    if(pTarget == nullptr || pSource == nullptr) {
        Logger::FormatErr("Invalid damage event: missing source (%d) or target (%d)", dmgEvt->m_idSource, dmgEvt->m_idTarget);
        return;
    }

    CHealthComponent* pHealthComponent = pTarget->GetComponent<CHealthComponent>();

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
    CGameObject* pSource = pGameState->FindGameObjectById(healEvt->m_idSource);
    CGameObject* pTarget = pGameState->FindGameObjectById(healEvt->m_idTarget);

    if(pTarget == nullptr || pSource == nullptr) {
        Logger::FormatErr("Invalid heal event: missing source (%d) or target (%d)", healEvt->m_idSource, healEvt->m_idTarget);
        return;
    }

    CHealthComponent* pHealthComponent = pTarget->GetComponent<CHealthComponent>();

    pHealthComponent->nHealth += healEvt->m_nHeal;
}