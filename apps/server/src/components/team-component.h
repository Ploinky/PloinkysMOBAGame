#pragma once

#include "GameObject.h"

class CTeamComponent : public IComponent{
public:
    CTeamComponent(Team eTeam) : eTeam(eTeam) {};

    Team eTeam = Team::NEUTRAL;
};