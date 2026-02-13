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

        pugi::xml_node ability = doc.child("ability");
        if(ability) {
            CAbilityData abilityData = LoadAbility(ability);
            gameData.mapAbilityData.emplace(abilityData.strId, abilityData);
            Logger::FormatMsg("Loaded ability data for %s from %s", abilityData.strId.c_str(), fileName.c_str());
            continue;
        }

        if(pugi::xml_node map = doc.child("map")) {
            CMapData mapData = LoadMapData(map);
            gameData.mapMapData.emplace(mapData.strId, mapData);
            continue;
        }

        if(pugi::xml_node triggerNode = doc.child("trigger")) {
            CTriggerData triggerData = LoadTriggerData(triggerNode);
            gameData.mapTriggerData.emplace(triggerData.id, triggerData);
            continue;
        }

        Logger::FormatMsg("Found no relevant data node in %s", fileName.c_str());
    }
    
    return gameData;
}


CTriggerData CServerDataLoader::LoadTriggerData(pugi::xml_node node) {
    CTriggerData data;

    data.id = node.attribute("id").as_string();

    for(pugi::xml_node spawnUnitNode : node.children("spawn_unit")) {
        CTriggerSpawnUnitData spawnUnitData;
        spawnUnitData.eTeam = spawnUnitNode.attribute("team").as_int() == 1 ? Team::TEAM_1 : Team::TEAM_2;
        spawnUnitData.idUnitType = spawnUnitNode.attribute("unit_type").as_string();
        spawnUnitData.nTime = spawnUnitNode.attribute("time").as_int();
        spawnUnitData.nCount = spawnUnitNode.attribute("count").as_int();
        data.vecSpawnUnitTriggers.push_back(spawnUnitData);
    }

    return data;
}