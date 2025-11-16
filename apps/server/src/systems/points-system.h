#pragma once

#include "system.h"
#include "events.h"

class CPointsSystem : public ISystem {
public:
    CPointsSystem();

    void OnDeath(CServerGameState* pGameState, CDeathEvent* pDeathEvt);

    unsigned int uTeam1Points = 0;
    unsigned int uTeam2Points = 0;
};