#pragma once

#include <vector>

class CSpawnUnitTrigger {
public:
    std::string idUnitType;
    Team eTeam;
    int nTime;
    int nCount;
    int nLastTime;
};

class CTrigger {
public:
    std::vector<CSpawnUnitTrigger> vecSpawnUnitTriggers;
};