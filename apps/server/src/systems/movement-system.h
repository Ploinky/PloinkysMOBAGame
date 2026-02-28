#pragma once

#include "common/game/game-system.h"
#include "events.h"

class CMovementSystem : public IGameSystem<CServerGameState> {
public:
    CMovementSystem();

    virtual void Update(CServerGameState* pGameState, float fDelta) override;
    virtual void UpdateEntity(CServerGameState* pGameState, float fDelta, MovementComponent_t& move) const;

    REGISTER_EVENT_HANDLER(CMovementSystem, CDeathEvent, OnDeath);
};