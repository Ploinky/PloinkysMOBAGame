#include "round-system.h"

void CRoundSystem::Update(CServerGameState* pGameState, float fDelta) {
    if (pGameState->eRoundState == ERoundState::POST_GAME) {
        m_fPostGameTimer += fDelta;
        if (m_fPostGameTimer >= m_fTotalPostGameTime) {
            pGameState->bIsGameEnded  = true;
        }
        return;
    }

    if (pGameState->eRoundState == ERoundState::PRE_ROUND) {
        m_fPreRoundTimer += fDelta;

        if (m_fPreRoundTimer >= m_fTotalPreRoundTime) {
            pGameState->eRoundState = ERoundState::SEARCH;
        }
        return;
    }

    if (pGameState->eRoundState == ERoundState::POST_ROUND) {
        m_fPostRoundTimer += fDelta;

        if (m_fPostRoundTimer >= m_fTotalPostRoundTime) {
            ResetRound(pGameState);
        }
        return;
    }

    int nTeam1Alive = 0;
    int nTeam2Alive = 0;

    for (auto& [id, team] : pGameState->GetAllTeam()) {
        if (HealthComponent_t* pHealth = pGameState->GetHealth(id)) {
            if (!pHealth ->bIsDead) {
                if (team.eTeam == Team::TEAM_1) {
                    nTeam1Alive++;
                } else if (team.eTeam == Team::TEAM_2) {
                    nTeam2Alive++;
                }
            }
        }
    }

    if (nTeam1Alive == 0) {
        RoundWon(pGameState, Team::TEAM_2);
    } else if (nTeam2Alive == 0) {
        RoundWon(pGameState, Team::TEAM_1);
    }
}

void CRoundSystem::ResetRound(CServerGameState* pGameState) {
    if (pGameState->uTeam1Points >= pGameState->nRoundWinsRequired) {
        pGameState->EmitEvent(CTeamWinEvent(Team::TEAM_1));
        pGameState->eRoundState = ERoundState::POST_GAME;
    } else if (pGameState->uTeam2Points >= pGameState->nRoundWinsRequired) {
        pGameState->EmitEvent(CTeamWinEvent(Team::TEAM_2));
        pGameState->eRoundState = ERoundState::POST_GAME;
    } else {
        pGameState->eRoundState = ERoundState::PRE_ROUND;
        pGameState->EmitEvent(CRoundResetEvent());
    }
}

void CRoundSystem::RoundWon(CServerGameState* pGameState, Team eTeam) {
    pGameState->EmitEvent(CRoundEndEvent(eTeam));
    pGameState->eRoundState = ERoundState::POST_ROUND;

    switch (eTeam) {
        case Team::TEAM_1:
            pGameState->uTeam1Points++;
            break;
        case Team::TEAM_2:
            pGameState->uTeam2Points++;
            break;
        case Team::NEUTRAL:
        default:
            break;
    }

    m_fPostRoundTimer = 0.0f;
}