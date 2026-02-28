#pragma once

#include "common/game/game-system.h"
#include "events.h"

class CTriggerSystem : public IGameSystem<CServerGameState> {
    virtual void Update(CServerGameState* pGameState, float fDelta) override;
};