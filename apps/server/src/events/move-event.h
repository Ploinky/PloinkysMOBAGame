#pragma once

#include "GameState.h"

class CMoveEvent : public IGameEvent {
public:
    CMoveEvent(UnitId idUnit, Vector3 vec3Position, float fRotation) : idUnit(idUnit), vec3Position(vec3Position), fRotation(fRotation) {};

    virtual std::type_index GetType() const override { return typeid(CMoveEvent); };

    UnitId idUnit;
    Vector3 vec3Position;
    float fRotation;
};