#include "points-system.h"

CPointsSystem::CPointsSystem() {
}

void CPointsSystem::Update(CServerGameState* pGameState, float fDelta) {
    if (pGameState->uTeam1Points >= 1 || pGameState->uTeam2Points >= 1) {
        m_fScoreboardTime -= fDelta;
        if (m_fScoreboardTime < 0) {
            pGameState->bIsGameEnded = true;
        }
    }
}

void CPointsSystem::OnDeath(CServerGameState* pGameState, const CDeathEvent& evt) {
    if(TeamComponent_t* pTeam = pGameState->GetTeam(evt.idTarget)) {
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

    if (pGameState->uTeam1Points >= 1) {
        pGameState->EmitEvent(CTeamWinEvent(Team::TEAM_1));
    } else if (pGameState->uTeam2Points >= 1) {
        pGameState->EmitEvent(CTeamWinEvent(Team::TEAM_2));
    }
}