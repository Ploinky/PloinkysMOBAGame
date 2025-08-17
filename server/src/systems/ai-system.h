#pragma once

#include "system.h"

class CAiSystem : public ISystem {
public:
    virtual void Update(CGameState* pGameState, float fDelta) override;
};