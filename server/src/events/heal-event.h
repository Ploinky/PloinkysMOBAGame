#pragma once

#include "GameState.h"
#include "components/Components.h"

class CHealEvent : public IGameEvent {
public:
    CHealEvent(UnitId idSource, UnitId idTarget, int nHeal);

    virtual void Execute(CGameState* pGameState) override;

    UnitId m_idSource;
    UnitId m_idTarget;
    int m_nHeal;
};