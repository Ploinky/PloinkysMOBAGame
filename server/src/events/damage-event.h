#pragma once

#include "GameState.h"

class CDamageEvent : public IGameEvent {
public:
    CDamageEvent(UnitId idSource, UnitId idTarget, int nDamage);

    virtual void Execute(CGameState* pGameState) override;

    UnitId m_idSource;
    UnitId m_idTarget;
    int m_nDamage;
};