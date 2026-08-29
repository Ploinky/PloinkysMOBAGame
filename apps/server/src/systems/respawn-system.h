#pragma once

#include <common/game/game-system.h>

#include "../events.h"

class CRespawnSystem : public IGameSystem<CServerGameState> {
public:
    CRespawnSystem();

    virtual void Update(CServerGameState* pGameState, float fDelta) override;

    REGISTER_EVENT_HANDLER(CRespawnSystem, CDeathEvent, OnEntityDied);
};