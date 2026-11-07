#pragma once

#include "../game/server-game-state.h"

class CTeamWinEvent : public IGameEvent {
public:
    CTeamWinEvent(Team eTeam) : eTeam(eTeam) {};

    virtual std::type_index GetType() const override { return typeid(CTeamWinEvent); };

    Team eTeam;
};