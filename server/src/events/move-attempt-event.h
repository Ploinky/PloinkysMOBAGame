#pragma once

#include "GameState.h"

class CMoveAttemptEvent : public IGameEvent {
public:
    CMoveAttemptEvent(UnitId idUnit, Vector3 vec3Position, float fRotation) : idUnit(idUnit), vec3Position(vec3Position), fRotation(fRotation) {};

    virtual std::type_index GetType() const override { return typeid(CMoveAttemptEvent); };

    UnitId idUnit;
    Vector3 vec3Position;
    float fRotation;
};