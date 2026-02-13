#pragma once

#include "system.h"
#include "events.h"

class CTriggerSystem : public ISystem {
    virtual void Update(CServerGameState* pGameState, float fDelta) override;
};