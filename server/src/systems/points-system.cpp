#include "points-system.h"

CPointsSystem::CPointsSystem() {
    REGISTER_EVENT_HANDLER(CDeathEvent, OnDeath);
}

void CPointsSystem::OnDeath(CGameState* pGameState, CDeathEvent* pDeathEvt) {
    CGameObject* pObj = pGameState->FindGameObjectById(pDeathEvt->idTarget);

    if(pObj == nullptr) {
        return;
    }

    pGameState->uTeam1Points += 1;
}