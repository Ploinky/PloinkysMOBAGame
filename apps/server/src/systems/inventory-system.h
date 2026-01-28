#pragma once

#include "system.h"

class CPickUpAttemptEvent;

class CInventorySystem : public ISystem {
public:
    CInventorySystem();

    virtual void Update(CServerGameState* pGameState, float fDelta) override;
    virtual void Finalize(CServerGameState* pGameState) override;
    void OnPickUpEntityAttempt(CServerGameState* pGameState, CPickUpAttemptEvent* pEvt);
};