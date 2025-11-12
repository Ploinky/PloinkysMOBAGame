#include "server-data-loader.h"

#include <iostream>
#include <sstream>

#include <common/logger.h>


pugi::xml_document CServerDataLoader::LoadXMLFile(std::string strFileName) {
    std::vector<uint8_t> mnfData = LoadFile(strFileName);

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_buffer(mnfData.data(), mnfData.size(), 116u, pugi::encoding_utf8);

    if(result.status != pugi::xml_parse_status::status_ok) {
        return {};
    }

    return doc;
}

CServerGameData CServerDataLoader::LoadManifest() {
    CServerGameData gameData{};

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

std::vector<float> ParseFloatVec(std::string str) {
    std::vector<float> result;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, ',')) {
        result.push_back(std::stof(item));
    }
    return result;
}
CAbilityData CServerDataLoader::LoadAbility(pugi::xml_node& abilityDataNode) {
    CAbilityData abilityData{};

    abilityData.strName = abilityDataNode.attribute("name").as_string();
    abilityData.strId = abilityDataNode.attribute("id").as_string();

    pugi::xml_node targetInfoNode = abilityDataNode.child("targeting_info");

    if(!targetInfoNode) {
        Logger::FormatErr("Failed to load ability %s: missing targeting info", abilityData.strId);
        return {};
    }

    std::string targetingType = targetInfoNode.attribute("type").as_string();

    if(targetingType == "unit") {
        abilityData.eTargetType = EAbilityTargetType::UNIT;
    } else {
        Logger::FormatErr("Failed to load ability %s: invalid targeting type", targetingType);
        return {};
    }

    abilityData.fCastRange = targetInfoNode.attribute("cast_range").as_int();
    abilityData.fCastTime = targetInfoNode.attribute("cast_time").as_int();
    abilityData.fCastPoint = targetInfoNode.attribute("cast_point").as_int();
    abilityData.fCooldown = targetInfoNode.attribute("fCooldown").as_int();

    pugi::xml_node onImpactNode = abilityDataNode.child("on_impact");

    if(onImpactNode) {
        for(pugi::xml_node onImpactEffectNode : onImpactNode.children()) {
            if(!strcmp(onImpactEffectNode.name(), "damage")) {
                ImpactEffectDamage_t effect{};

                if(strcmp(onImpactEffectNode.attribute("target").as_string(), "target_unit")) {
                    effect.eAffects = EImpactEffectAffects::TARGET_UNIT;
                }

                effect.vecDamage = ParseFloatVec(onImpactEffectNode.attribute("amount").as_string());

                abilityData.effect.vecDamageEffects.push_back(effect);
            }
        }
    }

    return abilityData;
}

CCharacterData CServerDataLoader::LoadCharacter(pugi::xml_node& characterNode) {
    CCharacterData charData{};
    charData.strId = characterNode.attribute("id").as_string();
    
    pugi::xml_document doc = LoadXMLFile("data/characters/" + charData.strId + "/" + charData.strId + ".xml");

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
            if(strcmp(abilityNode.name(), "ability") != 0) {
                continue;
            }

            int slot = abilityNode.attribute("slot").as_int();
            std::string abilityId = abilityNode.attribute("id").as_string();
            charData.mapAbilityIds.emplace(slot, abilityId);
        }
    }

    return charData;
}