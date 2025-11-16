#pragma once

#include "system.h"

class CServerGameState;

class CAiSystem : public ISystem {
public:
    virtual void Update(CServerGameState* pGameState, float fDelta) override;
};