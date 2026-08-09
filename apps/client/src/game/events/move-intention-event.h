#pragma once

#include "../client-game-state.h"

class CMoveIntentionEvent : public IGameEvent {
public:
    CMoveIntentionEvent() {};

    virtual std::type_index GetType() const override { return typeid(CMoveIntentionEvent); };

    UnitId idUnit;
};