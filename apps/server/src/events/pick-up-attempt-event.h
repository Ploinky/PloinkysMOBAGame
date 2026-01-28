#pragma once

#include "game/server-game-state.h"

class CPickUpAttemptEvent : public IGameEvent {
public:
    CPickUpAttemptEvent(UnitId idUnit, UnitId idTargetUnit) : idUnit(idUnit), idTargetUnit(idTargetUnit) {};

    virtual std::type_index GetType() const override { return typeid(CPickUpAttemptEvent); };

    UnitId idUnit;
    UnitId idTargetUnit;
};