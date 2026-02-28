#pragma once

#include "common/game/game-system.h"
#include "events.h"

class CServerGameState;

class CAiSystem : public IGameSystem<CServerGameState> {
public:
    CAiSystem();

    virtual void Update(CServerGameState* pGameState, float fDelta) override;

    REGISTER_EVENT_HANDLER(CAiSystem, CNavDestEvent, OnNavDest);
};