#pragma once

#include <vector>

#include "common/PMG_Common.h"

class CSpawnUnitTrigger {
public:
    std::string idUnitType;
    Team eTeam;
    int nTime;
    int nCount;
    int nLastTime;
    Vector2 vec2Position;
};

class CTrigger {
public:
    std::vector<CSpawnUnitTrigger> vecSpawnUnitTriggers;
};