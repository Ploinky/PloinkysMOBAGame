#pragma once

#include "system.h"

class CMovementSystem : public ISystem {
public:
    virtual void Process(CGameState* pGameState, float fDelta);
};