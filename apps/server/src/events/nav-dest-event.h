#pragma once

#include "game/server-game-state.h"

class CNavDestEvent : public IGameEvent {
public:
    CNavDestEvent(UnitId idUnit, Vector3 vec3Target) : idUnit(idUnit), vec3Target(vec3Target) {};

    virtual std::type_index GetType() const override { return typeid(CNavDestEvent); };

    UnitId idUnit;
    Vector3 vec3Target;
};