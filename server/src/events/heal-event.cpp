#include "heal-event.h"

CHealEvent::CHealEvent(UnitId idSource, UnitId idTarget, int nHeal) {
    m_idSource = idSource;
    m_idTarget = idTarget;
    m_nHeal = nHeal;
}

void CHealEvent::Execute(CGameState* pGameState) {
    CGameObject* pTarget = pGameState->FindGameObjectById(m_idTarget);

    if(pTarget == nullptr) {
        Logger::FormatErr("Invalid heal command: missing target unit %d", m_idTarget);
        return;
    }

    CHealthComponent* pHealth = pTarget->GetComponent<CHealthComponent>();

    if(pHealth == nullptr) {
        Logger::FormatErr("Invalid heal command: target unit %d is missing health component", m_idTarget);
        return;
    }

    pHealth->nHealth += m_nHeal;
}