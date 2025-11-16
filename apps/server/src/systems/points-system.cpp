#include "points-system.h"

CPointsSystem::CPointsSystem() {
    REGISTER_EVENT_HANDLER(CDeathEvent, OnDeath);
}

void CPointsSystem::OnDeath(CServerGameState* pGameState, CDeathEvent* pDeathEvt) {
    CGameObject* pObj = pGameState->FindGameObjectById(pDeathEvt->idTarget);

    if(pObj == nullptr) {
        return;
    }

    if(CTeamComponent* pTeam = pObj->GetComponent<CTeamComponent>()) {
        switch(pTeam->eTeam) {
            case Team::TEAM_1:
                pGameState->uTeam2Points += 1;
                break;
            case Team::TEAM_2:
                pGameState->uTeam1Points += 1;
                break;
            case Team::NEUTRAL:
            default:
                break;
        }
    }
}