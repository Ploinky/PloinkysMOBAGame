#include "points-system.h"

CPointsSystem::CPointsSystem() {
}

void CPointsSystem::OnDeath(CServerGameState* pGameState, CDeathEvent* pDeathEvt) {
    if(TeamComponent_t* pTeam = pGameState->GetTeam(pDeathEvt->idTarget)) {
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