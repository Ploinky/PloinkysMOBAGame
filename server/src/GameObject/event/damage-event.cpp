#include "damage-event.h"
#include "GameObject/GameObject.h"
#include "GameObject/Components.h"

CDamageCommand::CDamageCommand(UnitId idSource, UnitId idTarget, int nDamage) {
    m_idSource = idSource;
    m_idTarget = idTarget;
    m_nDamage = nDamage;
}

void CDamageCommand::Execute(CGameState* pGameState) {
    CGameObject* pSource = pGameState->FindGameObjectById(m_idSource);
    CGameObject* pTarget = pGameState->FindGameObjectById(m_idTarget);

    if(pSource == nullptr || pTarget == nullptr) {
        Logger::FormatErr("Invalid damage command: source (%d) or target (%d) unit missing", m_idSource, m_idTarget);
        return;
    }

    pTarget->GetComponent<CHealthComponent>()->TakeDamage(m_nDamage);

    Logger::FormatMsg("Apply %d damage to %d from %d", m_nDamage, m_idSource, m_idTarget);
}