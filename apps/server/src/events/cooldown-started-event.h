#pragma once

#include "GameState.h"

class CCooldownStartedEvent : public IGameEvent {
public:
    CCooldownStartedEvent(UnitId idUnit, int nSpellIndex, float fCooldown) : idUnit(idUnit), nSpellIndex(nSpellIndex), fCooldown(fCooldown) {};
    virtual std::type_index GetType() const override { return typeid(CCooldownStartedEvent); };

    UnitId idUnit;
    int nSpellIndex;
    float fCooldown;
};