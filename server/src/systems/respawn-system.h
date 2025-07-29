#pragma once

#include "system.h"
#include "events.h"

class CRespawnSystem : public ISystem {
public:
    CRespawnSystem();

    virtual void Update(CGameState* pGameState, float fDelta) override;
};