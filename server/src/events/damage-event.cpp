#include "damage-event.h"
#include "GameObject.h"
#include "components/Components.h"

CDamageEvent::CDamageEvent(UnitId idSource, UnitId idTarget, int nDamage) {
    m_idSource = idSource;
    m_idTarget = idTarget;
    m_nDamage = nDamage;
}

void CDamageEvent::Execute(CGameState* pGameState) {
    CGameObject* pSource = pGameState->FindGameObjectById(m_idSource);
    CGameObject* pTarget = pGameState->FindGameObjectById(m_idTarget);

    if(pSource == nullptr || pTarget == nullptr) {
        Logger::FormatErr("Invalid damage command: source (%d) or target (%d) unit missing", m_idSource, m_idTarget);
        return;
    }

    pTarget->GetComponent<CHealthComponent>()->nHealth -= m_nDamage;

    Logger::FormatMsg("Apply %d damage to %d from %d", m_nDamage, m_idSource, m_idTarget);
}