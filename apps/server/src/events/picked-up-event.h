#pragma once

#include "game/server-game-state.h"

class CPickedUpEvent : public IGameEvent {
public:
    CPickedUpEvent(UnitId idUnit, UnitId idTargetUnit) : idUnit(idUnit), idTargetUnit(idTargetUnit) {};

    virtual std::type_index GetType() const override { return typeid(CPickedUpEvent); };

    UnitId idUnit;
    UnitId idTargetUnit;
};