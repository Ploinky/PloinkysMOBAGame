#include "AssetManager.h"
#include <fstream>
#include <iostream>
#include "util.h"
#include <exception>
#include <memory.h>
#include <cstdint>
#include "logger.h"
#include <filesystem>


AssetManager::AssetManager() {
}

AssetManager::~AssetManager() {
	for (paklib::PakFile* pakFile : files) {
		delete pakFile;
	}
}

void AssetManager::LoadPakFile(std::string fileName) {
	files.push_back(paklib::PakFile::Load(fileName));
}

std::vector<uint8_t> AssetManager::LoadFile(std::string fileName) {
#ifdef _DEBUG
	std::ifstream inputFile(fileName, std::ios_base::binary);

    inputFile.seekg(0, std::ios_base::end);
    auto length = inputFile.tellg();
    inputFile.seekg(0, std::ios_base::beg);

    if(length == -1) {
        return {};
    }

    // Make a buffer of the exact size of the file and read the data into it.
    std::vector<uint8_t> buffer(length);
    inputFile.read(reinterpret_cast<char*>(buffer.data()), length);

    inputFile.close();
    return buffer;
#else
	// check if any of our paks have the file
	for (paklib::PakFile* pakFile : files) {
		if (pakFile->HasFile(fileName)) {
			return pakFile->GetFileData(fileName);
		}
	}
#endif
	Logger::FormatErr("Failed to load asset <%s>", fileName.c_str());
	return {};
}

std::list<std::string> AssetManager::LoadPlainFile(std::string fileName) {
	std::vector<uint8_t> bytes = LoadFile(fileName);

	std::list<std::string> content;
	std::string currLine = "";

	for (int i = 0; i < bytes.size(); i++) {
		char c = bytes[i];

		if (c == '\n' || c == '\r') {
			if (currLine.length() > 0) {
				content.push_back(currLine);
			}
			currLine = "";
		}
		else {
			currLine += c;
		}
	}
	
	if (currLine.length() > 0) {
		content.push_back(currLine);
	}

	return content;
}


std::vector<std::string> AssetManager::GetFileNamesByExtension(const std::string strPathToSearch, const std::string strFileEnding) {
    std::vector<std::string> vecFileNames;

    for(std::filesystem::directory_entry entry : std::filesystem::directory_iterator(strPathToSearch)) {
        if(entry.is_directory()) {
            std::vector<std::string> vecFoundInDir = GetFileNamesByExtension(entry.path().string(), strFileEnding);
            if(!vecFoundInDir.empty()) {
                vecFileNames.insert(vecFileNames.end(), vecFoundInDir.begin(), vecFoundInDir.end());
            }
        }

        if(entry.is_regular_file() && !entry.path().extension().string().compare(strFileEnding)) {
            vecFileNames.push_back(strPathToSearch + "/" + entry.path().filename().string());
        }
    }

    return vecFileNames;
}

CAbilityData AssetManager::LoadAbility(pugi::xml_node& abilityDataNode) {
    CAbilityData abilityData{};

    abilityData.strName = abilityDataNode.attribute("name").as_string();
    abilityData.strId = abilityDataNode.attribute("id").as_string();
    abilityData.iconId = abilityDataNode.attribute("icon").as_string();

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
            if(!strcmp(onImpactEffectNode.name(), "heal")) {
                ImpactEffectHeal_t effect{};

                if(strcmp(onImpactEffectNode.attribute("target").as_string(), "target_unit")) {
                    effect.eAffects = EImpactEffectAffects::TARGET_UNIT;
                }

                effect.vecHeal = ParseFloatVec(onImpactEffectNode.attribute("amount").as_string());

                abilityData.effect.vecHealEffects.push_back(effect);
            }

            if(!strcmp(onImpactEffectNode.name(), "effect")) {
                ImpactEffectFX_t effect{};

                effect.strId = onImpactEffectNode.attribute("id").as_string();

                abilityData.effect.vecFXEffects.push_back(effect);
            }
        }
    }

    return abilityData;
}

pugi::xml_document AssetManager::LoadXMLFile(std::string strFileName) {
    std::vector<uint8_t> mnfData = LoadFile(strFileName);

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_buffer(mnfData.data(), mnfData.size(), 116u, pugi::encoding_utf8);

    if(result.status != pugi::xml_parse_status::status_ok) {
        return {};
    }

    return doc;
}

CCharacterData AssetManager::LoadCharacter(pugi::xml_node& charDataNode) {
    CCharacterData charData{};
    charData.strId = charDataNode.attribute("id").as_string();
    

    if(!charDataNode) {
        Logger::FormatErr("Failed to load character data file for %s: missing character_data node", charData.strId);
        return {};
    }

    charData.strName = charDataNode.attribute("name").as_string();

    if(charDataNode.attribute("model")) {
        std::string modelId = charDataNode.attribute("model").as_string();
        charData.modelId = modelId;
    }

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

    pugi::xml_node transformNode = charDataNode.child("transform");
    if(transformNode) {
        CTransformData transformData{};
        int nCollisionRadius = transformNode.attribute("collision_radius").as_int();
        transformData.nCollisionRadius = nCollisionRadius;
        charData.optTransformData.emplace(transformData);
    }

    pugi::xml_node networkNode = charDataNode.child("network");
    if(networkNode) {
        CNetworkData networkData{};
        bool bSyncMovement = networkNode.attribute("sync_movement").as_bool();
        networkData.bSyncMovement = bSyncMovement;
        charData.optNetworkData.emplace(networkData);
    }

    pugi::xml_node movementNode = charDataNode.child("movement");
    if(movementNode) {
        CMovementData movementData{};
        movementData.nSpeed = movementNode.attribute("speed").as_int();
        charData.optMovementData.emplace(movementData);
    }

    pugi::xml_node healthNode = charDataNode.child("health");
    if(healthNode) {
        CHealthData healthData{};
        healthData.nMaxHealth = healthNode.attribute("max_health").as_int();
        charData.optHealthData.emplace(healthData);
    }

    pugi::xml_node navigationNode = charDataNode.child("navigation");
    if(navigationNode) {
        CNavigationData navigationData{};
        charData.optNavigationData.emplace(navigationData);
    }

    pugi::xml_node intentNode = charDataNode.child("intent");
    if(intentNode) {
        charData.optIntentData.emplace(CIntentData());
    }

    pugi::xml_node targetableNode = charDataNode.child("targetable");
    if(targetableNode) {
        charData.optTargetableData.emplace(CTargetableData());
    }

    pugi::xml_node audioNode = charDataNode.child("audio");
    if(audioNode) {
        CAudioData audioData = CAudioData();
        pugi::xml_attribute deathAudio = audioNode.attribute("death");
        if(deathAudio) {
            audioData.mapAudioIds.emplace("death", deathAudio.as_string());
        }
        pugi::xml_attribute attack1Audio = audioNode.attribute("attack1");
        if(attack1Audio) {
            audioData.mapAudioIds.emplace("attack1", attack1Audio.as_string());
        }
        charData.optAudioData.emplace(audioData);
    }

    return charData;
}

CMapData AssetManager::LoadMapData(pugi::xml_node& mapNode) {
    CMapData mapData;

    mapData.strId = mapNode.attribute("id").as_string();
    
    if(pugi::xml_node navMeshNode = mapNode.child("navmesh")) {
        mapData.navMeshDataFile = navMeshNode.attribute("file").as_string();
    }

    if(pugi::xml_node camVolumeNode = mapNode.child("camvolume")) {
        mapData.camVolume.fBack = camVolumeNode.attribute("back").as_float();
        mapData.camVolume.fFront = camVolumeNode.attribute("front").as_float();
        mapData.camVolume.fLeft = camVolumeNode.attribute("left").as_float();
        mapData.camVolume.fRight = camVolumeNode.attribute("right").as_float();
        mapData.camVolume.fTop = camVolumeNode.attribute("top").as_float();
        mapData.camVolume.fBottom = camVolumeNode.attribute("bottom").as_float();
    }

    return mapData;
}

std::vector<float> AssetManager::ParseFloatVec(std::string str) {
    std::vector<float> result;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, ',')) {
        result.push_back(std::stof(item));
    }
    return result;
}