#include "spell-cast-api.h"

#include "GameObject.h"
#include "components/Components.h"
#include "events.h"

CSpellCastApi::CSpellCastApi(CGameState* pGameState) {
    m_pGameState = pGameState;
}

void CSpellCastApi::ApplyDamage(UnitId idSource, UnitId idTarget, float fDamage) {
    CGameObject* pTarget = m_pGameState->FindGameObjectById(idTarget);

    CHealthComponent* pHealth = pTarget->GetComponent<CHealthComponent>();

    if(pHealth == nullptr) {
        return;
    }

    pHealth->nHealth -= fDamage;

    m_pGameState->VecEvent.emplace(new CDamageEvent(idSource, idTarget, fDamage));
}