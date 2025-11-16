#include "server-data-loader.h"

#include <iostream>
#include <sstream>

#include <common/logger.h>

CGameData CServerDataLoader::LoadManifest() {
    CGameData gameData{};

    Logger::FormatMsg("Attempting to load game data");

    for(std::string fileName : GetFileNamesByExtension("data", ".xml")) {
        pugi::xml_document doc = LoadXMLFile(fileName);

        pugi::xml_node character = doc.child("entity");
        if(character) {
            CCharacterData charData = LoadCharacter(character);
            gameData.mapCharacterData.emplace(charData.strId, charData);
            Logger::FormatMsg("Loaded character data for %s from %s", charData.strId.c_str(), fileName.c_str());
            continue;
        }

        pugi::xml_node ability = doc.child("ability_data");
        if(ability) {
            CAbilityData abilityData = LoadAbility(ability);
            gameData.mapAbilityData.emplace(abilityData.strId, abilityData);
            Logger::FormatMsg("Loaded ability data for %s from %s", abilityData.strId.c_str(), fileName.c_str());
            continue;
        }

        Logger::FormatMsg("Found no relevant data node in %s", fileName.c_str());
    }
    
    return gameData;
}