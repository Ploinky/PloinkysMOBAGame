#pragma once

#include "GameState.h"

class CDamageEvent : public IGameEvent {
public:
    CDamageEvent(UnitId idSource, UnitId idTarget, int nDamage);

    virtual std::type_index GetType() const override { return typeid(CDamageEvent); };

    UnitId m_idSource;
    UnitId m_idTarget;
    int m_nDamage;
};