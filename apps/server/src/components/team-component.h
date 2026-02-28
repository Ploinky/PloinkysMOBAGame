#pragma once

#include "common/pmg_types.h"

typedef struct TeamComponent_t {
    TeamComponent_t() : TeamComponent_t(Team::NEUTRAL) {};
    TeamComponent_t(Team eTeam) : eTeam(eTeam) {};
    
    UnitId idUnit;
    Team eTeam = Team::NEUTRAL;
} TeamComponent_t;