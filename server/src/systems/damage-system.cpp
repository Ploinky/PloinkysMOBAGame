#include "damage-system.h"

#include "GameState.h"
#include "events/damage-event.h"
#include "events/heal-event.h"
#include "GameObject.h"
#include "components/Components.h"

void CDamageSystem::Process(CGameState* pGameState, float fDelta) {
    for(CDamageEvent* dmgEvt : pGameState->GetEvents<CDamageEvent>()) {
        CGameObject* pSource = pGameState->FindGameObjectById(dmgEvt->m_idSource);
        CGameObject* pTarget = pGameState->FindGameObjectById(dmgEvt->m_idTarget);

        if(pTarget == nullptr || pSource == nullptr) {
            Logger::FormatErr("Invalid damage event: missing source (%d) or target (%d)", dmgEvt->m_idSource, dmgEvt->m_idTarget);
            continue;
        }

        CHealthComponent* pHealthComponent = pTarget->GetComponent<CHealthComponent>();

        pHealthComponent->nHealth -= dmgEvt->m_nDamage;
    }

    for(CHealEvent* dmgEvt : pGameState->GetEvents<CHealEvent>()) {
        CGameObject* pSource = pGameState->FindGameObjectById(dmgEvt->m_idSource);
        CGameObject* pTarget = pGameState->FindGameObjectById(dmgEvt->m_idTarget);

        if(pTarget == nullptr || pSource == nullptr) {
            Logger::FormatErr("Invalid heal event: missing source (%d) or target (%d)", dmgEvt->m_idSource, dmgEvt->m_idTarget);
            continue;
        }

        CHealthComponent* pHealthComponent = pTarget->GetComponent<CHealthComponent>();

        pHealthComponent->nHealth += dmgEvt->m_nHeal;
    }
}