#pragma once

#include "system.h"

class CDamageSystem : public ISystem {
public:
    virtual void Process(CGameState* pGameState, float fDelta) override;
};