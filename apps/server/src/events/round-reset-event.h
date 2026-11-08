#pragma once

#include "../game/server-game-state.h"

class CRoundResetEvent : public IGameEvent {
public:
    CRoundResetEvent() {};

    virtual std::type_index GetType() const override { return typeid(CRoundResetEvent); };
};