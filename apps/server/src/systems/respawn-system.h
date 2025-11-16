#pragma once

#include "system.h"
#include "events.h"

class CRespawnSystem : public ISystem {
public:
    CRespawnSystem();

    virtual void Update(CServerGameState* pGameState, float fDelta) override;
};