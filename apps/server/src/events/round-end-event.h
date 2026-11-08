#pragma once

#include "../game/server-game-state.h"

class CRoundEndEvent : public IGameEvent {
public:
    CRoundEndEvent(Team eWinner) : eWinner(eWinner) {};

    virtual std::type_index GetType() const override { return typeid(CRoundEndEvent); };

    Team eWinner;
};