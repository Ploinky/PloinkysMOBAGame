#pragma once

#include "system.h"

class CSpellSystem : public ISystem {
public:
    virtual void Process(CGameState* pGameState, float fDelta) override;
};