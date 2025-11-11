#include "server-data-loader.h"

#include <iostream>

#include <common/logger.h>


CServerDataLoader::CServerDataLoader(std::shared_ptr<AssetManager> pAssetManager) {
    m_pAssetManager = pAssetManager;
}

pugi::xml_document CServerDataLoader::LoadFile(std::string strFileName) {
    std::vector<uint8_t> mnfData = m_pAssetManager->LoadFile(strFileName);

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_buffer(mnfData.data(), mnfData.size(), 116u, pugi::encoding_utf8);

    if(result.status != pugi::xml_parse_status::status_ok) {
        return {};
    }

    return doc;
}

CServerGameData CServerDataLoader::LoadManifest() {
    CServerGameData gameData{};

    pugi::xml_document doc = LoadFile("mnf.xml");
    if(!doc) {
        Logger::FormatErr("Failed to load data manifest file");
        return {};
    }

    pugi::xml_node gameDataNode = doc.child("game_data");

    if(!gameDataNode) {
        Logger::FormatErr("Failed to load data manifest file: missing game_data node");
        return {};
    }

    pugi::xml_node characters = gameDataNode.child("characters");

    if(characters) {
        for(pugi::xml_node character : characters.children()) {
            CCharacterData charData = LoadCharacter(character);
            gameData.mapCharacterData.emplace(charData.strId, charData);
        }
    }
    
    return gameData;
}

CAbilityData CServerDataLoader::LoadAbility(std::string charId, pugi::xml_node& abilityNode) {
    CAbilityData abilityData{};
    abilityData.strId = abilityNode.attribute("id").as_string();
    pugi::xml_document doc = LoadFile("characters/" + charId + "/abilities/" + abilityData.strId + "/" + abilityData.strId + ".xml");
    
    if(!doc) {
        Logger::FormatErr("Failed to load ability data file for %s", abilityData.strId);
        return {};
    }

    pugi::xml_node abilityDataNode = doc.child("ability_data");
    abilityData.strName = abilityDataNode.attribute("name").as_string();

    return abilityData;
}

CCharacterData CServerDataLoader::LoadCharacter(pugi::xml_node& characterNode) {
    CCharacterData charData{};
    charData.strId = characterNode.attribute("id").as_string();
    
    pugi::xml_document doc = LoadFile("characters/" + charData.strId + "/" + charData.strId + ".xml");

    if(!doc) {
        Logger::FormatErr("Failed to load character data file for %s", charData.strId);
        return {};
    }

    pugi::xml_node charDataNode = doc.child("character_data");
    
    if(!charDataNode) {
        Logger::FormatErr("Failed to load character data file for %s: missing character_data node", charData.strId);
        return {};
    }

    charData.strName = charDataNode.attribute("name").as_string();

    pugi::xml_node abilitiesNode = charDataNode.child("abilities");

    if(abilitiesNode) {
        for(pugi::xml_node abilityNode : abilitiesNode.children()) {
            if(abilitiesNode.name() != "ability") {
                continue;
            }

            int slot = abilityNode.attribute("slot").as_int();
            CAbilityData ability = LoadAbility(charData.strId, abilityNode);
            charData.mapAbilities.emplace(slot, ability);
        }
    }

    return charData;
}