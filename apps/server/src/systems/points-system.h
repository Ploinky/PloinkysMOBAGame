#pragma once

#include <common/game/game-system.h>

#include "../events.h"

class CPointsSystem : public IGameSystem<CServerGameState> {
public:
    CPointsSystem();

    virtual void Update(CServerGameState* pGameState, float fDelta) override;

    REGISTER_EVENT_HANDLER(CPointsSystem, CDeathEvent, OnDeath);

    unsigned int uTeam1Points = 0;
    unsigned int uTeam2Points = 0;

private:
    float m_fScoreboardTime = 10000.0f;
};