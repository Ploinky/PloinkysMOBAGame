#pragma once

#include "GameState.h"

class CAttackIntentionEvent : public IGameEvent {
public:
    CAttackIntentionEvent(UnitId idUnit, UnitId idTarget) : idUnit(idUnit), idTarget(idTarget) {};
    virtual std::type_index GetType() const override { return typeid(CAttackIntentionEvent); };

    UnitId idUnit;
    UnitId idTarget;
};