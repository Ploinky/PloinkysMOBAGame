#include "server-data-loader.h"

#include <iostream>
#include <sstream>

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

    pugi::xml_document doc = LoadFile("data/mnf.xml");
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

std::vector<float> ParseFloatVec(std::string str) {
    std::vector<float> result;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, ',')) {
        result.push_back(std::stof(item));
    }
    return result;
}
CAbilityData CServerDataLoader::LoadAbility(std::string charId, pugi::xml_node& abilityNode) {
    CAbilityData abilityData{};
    abilityData.strId = abilityNode.attribute("id").as_string();
    pugi::xml_document doc = LoadFile("data/characters/" + charId + "/abilities/" + abilityData.strId + "/" + abilityData.strId + ".xml");
    
    if(!doc) {
        Logger::FormatErr("Failed to load ability data file for %s", abilityData.strId);
        return {};
    }

    pugi::xml_node abilityDataNode = doc.child("ability_data");
    abilityData.strName = abilityDataNode.attribute("name").as_string();

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
    
    pugi::xml_document doc = LoadFile("data/characters/" + charData.strId + "/" + charData.strId + ".xml");

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
            CAbilityData ability = LoadAbility(charData.strId, abilityNode);
            charData.mapAbilities.emplace(slot, ability);
        }
    }

    return charData;
}