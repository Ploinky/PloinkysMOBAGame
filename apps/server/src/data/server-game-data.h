#pragma once

#include <string>
#include <map>

class CAbilityData {
public:
    std::string strId;
    std::string strName;
};

class CCharacterData {
public:
    std::string strId;
    std::string strName;
    std::map<int, CAbilityData> mapAbilities;
};

class CServerGameData {
public:
    std::map<std::string, CCharacterData> mapCharacterData;
};