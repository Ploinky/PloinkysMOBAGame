#pragma once

#include "common/game/game-system.h"

class CPickUpAttemptEvent;
class CUseEntityAttemptEvent;
class CServerGameState;

class CInventorySystem : public IGameSystem<CServerGameState> {
public:
    CInventorySystem();

    virtual void Update(CServerGameState* pGameState, float fDelta) override;
    virtual void Finalize(CServerGameState* pGameState) override;

    REGISTER_EVENT_HANDLER(CInventorySystem, CPickUpAttemptEvent, OnPickUpEntityAttempt);
    REGISTER_EVENT_HANDLER(CInventorySystem, CUseEntityAttemptEvent, OnUseEntityAttempt);
};