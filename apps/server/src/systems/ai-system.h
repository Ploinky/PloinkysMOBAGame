#pragma once

#include "system.h"
#include "events.h"

class CServerGameState;

class CAiSystem : public ISystem {
public:
    CAiSystem();

    virtual void Update(CServerGameState* pGameState, float fDelta) override;

    void OnMove(CServerGameState* pGameState, CMoveEvent* pMoveEvent);
};