#include "cleanup-system.h"

void CCleanupSystem::Update(CServerGameState* pGameState, float fDelta) {
    for (UnitId id : m_vecUnitsToCleanUp) {
        CleanupUnit(pGameState, id);
    }

    m_vecUnitsToCleanUp.clear();
}

void CCleanupSystem::OnUnitDied(CServerGameState* pGameState, const CDeathEvent& evt) {
    if (RespawnComponent_t* pRespawn = pGameState->GetRespawn(evt.idTarget)) {
        return;
    }

    m_vecUnitsToCleanUp.push_back(evt.idTarget);
}


void CCleanupSystem::CleanupUnit(CServerGameState* pGameState, UnitId id) {
    pGameState->DeleteEntity(id);

    pGameState->EmitEvent(CDespawnEvent(id));
}