#pragma once

#include <vector>

class CSpawnUnitTrigger {
public:
    std::string idUnitType;
    Team eTeam;
};

class CTrigger {
public:
    std::vector<CSpawnUnitTrigger> vecSpawnUnitTriggers;
};