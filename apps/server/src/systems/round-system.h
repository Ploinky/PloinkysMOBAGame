#pragma once

#include "common/game/game-system.h"
#include "common/data/game-data.h"
#include "events.h"

class CRoundSystem : public IGameSystem<CServerGameState> {
public:
    virtual void Update(CServerGameState* pGameState, float fDelta) override;

private:
    virtual void RoundWon(CServerGameState* pGameState, Team eTeam);
    virtual void ResetRound(CServerGameState* pGameState);

    float m_fTotalPostRoundTime = 3000.0f;
    float m_fPostRoundTimer = 0.0f;
    float m_fTotalPreRoundTime = 3000.0f;
    float m_fPreRoundTimer = 0.0f;
    float m_fTotalRoundTime = 45000.0f;
    float m_fRoundTimer = 0.0f;
    float m_fTotalPostGameTime = 5000.0f;
    float m_fPostGameTimer = 0.0f;
};
