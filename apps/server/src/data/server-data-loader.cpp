#include "server-data-loader.h"

#include <iostream>
#include <sstream>

#include <common/logger.h>

CGameData CServerDataLoader::LoadManifest() {
    CGameData gameData{};

    for(std::string chrFileNames : GetFileNamesByExtension("data", ".chr")) {
        pugi::xml_document doc = LoadXMLFile(chrFileNames);
        pugi::xml_node character = doc.child("character_data");
        CCharacterData charData = LoadCharacter(character);
        gameData.mapCharacterData.emplace(charData.strId, charData);
    }

    for(std::string ablFileNames : GetFileNamesByExtension("data", ".abl")) {
        pugi::xml_document doc = LoadXMLFile(ablFileNames);
        pugi::xml_node rootNode = doc.child("ability_data");
        CAbilityData abilityData = LoadAbility(rootNode);
        gameData.mapAbilityData.emplace(abilityData.strId, abilityData);
    }
    
    return gameData;
}