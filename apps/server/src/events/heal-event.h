#pragma once

#include "game/server-game-state.h"
#include "components/Components.h"

class CHealEvent : public IGameEvent {
public:
    CHealEvent(UnitId idSource, UnitId idTarget, int nHeal);

    virtual std::type_index GetType() const override { return typeid(CHealEvent); };

    UnitId m_idSource;
    UnitId m_idTarget;
    int m_nHeal;
};