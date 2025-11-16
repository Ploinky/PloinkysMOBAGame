#pragma once

#include "system.h"
#include "events.h"

class CMovementSystem : public ISystem {
public:
    CMovementSystem();

    virtual void Update(CServerGameState* pGameState, float fDelta) override;

    void OnDeath(CServerGameState* pGameState, CDeathEvent* pDeathEvt);
};