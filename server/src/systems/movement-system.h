#pragma once

#include "system.h"
#include "events.h"

class CMovementSystem : public ISystem {
public:
    CMovementSystem();

    virtual void Update(CGameState* pGameState, float fDelta) override;

    void OnDeath(CGameState* pGameState, CDeathEvent* pDeathEvt);
};