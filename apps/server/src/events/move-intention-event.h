#pragma once

#include "GameState.h"

class CMoveIntentionEvent : public IGameEvent {
public:
    CMoveIntentionEvent(UnitId idUnit, Vector3 vec3Position, float fRotation) : idUnit(idUnit), vec3Position(vec3Position), fRotation(fRotation) {};

    virtual std::type_index GetType() const override { return typeid(CMoveIntentionEvent); };

    UnitId idUnit;
    Vector3 vec3Position;
    float fRotation;
};