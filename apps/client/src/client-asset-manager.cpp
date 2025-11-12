#include "client-asset-manager.h"

#include "common/PMG_Common.h"
#include "Model.h"

#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb_image.h"

#define CLEANUP(res) if(res != nullptr) { res->Release(); res = nullptr;}

CClientAssetManager::CClientAssetManager(IGraphicsEngine* pGraphicsEngine, IAudioEngine* pAudioEngine) {
    m_pGraphicsEngine = pGraphicsEngine;
    m_pAudioEngine = pAudioEngine;
}

void CClientAssetManager::Cleanup() {
}


HBitmap CClientAssetManager::GetBitmapImage(std::string strBitmap) {
    if(m_mapBitmaps.contains(strBitmap)) {
        return m_mapBitmaps.at(strBitmap);
    }

    std::vector<uint8_t> vecImageData = LoadFile(strBitmap);

    int width, height, channels_in_file;
    unsigned char *image = stbi_load_from_memory(
        vecImageData.data(),
        vecImageData.size(),
        &width,           // output for width
        &height,          // output for height
        &channels_in_file, // output for original channels in file
        4                 // desired number of channels
    );

    HBitmap hBitmap = m_pGraphicsEngine->LoadBitmapImage(image, width, height);

    if(hBitmap == INVALID_ASSET_HANDLE) {
        Logger::FormatErr("Missing asset %s", strBitmap.c_str());
    } else {
        m_mapBitmaps.emplace(strBitmap, hBitmap);
    }

    return hBitmap;
}

HTexture CClientAssetManager::LoadTextureFromData(std::vector<uint8_t> imageData) {
    int width, height, channels_in_file;
    unsigned char* pImageData = stbi_load_from_memory(
        imageData.data(),
        imageData.size(),
        &width,
        &height,
        &channels_in_file,
        4
    );

    HTexture hTexture = m_pGraphicsEngine->LoadTexture(pImageData, width, height);

    return hTexture;
}

HTexture CClientAssetManager::LoadTexture(std::string strTexture) {
    if(m_mapTextures.contains(strTexture)) {
        return m_mapTextures.at(strTexture);
    }

    std::vector<uint8_t> imageData = LoadFile(strTexture);
    HTexture hTexture = LoadTextureFromData(imageData);
    m_mapTextures.emplace(strTexture, hTexture);
    return hTexture;
}
HSound CClientAssetManager::LoadSound(std::string strSound) {
    if(m_mapSounds.contains(strSound)) {
        return m_mapSounds[strSound];
    }
    
    std::vector<uint8_t> vecFileData = LoadFile(strSound);

    if (vecFileData.size() == 0) {
        Logger::Err("Failed to load sound");
        return INVALID_ASSET_HANDLE;
    }

    return m_pAudioEngine->LoadSound(vecFileData);
}

HModel CClientAssetManager::LoadModel(std::string strModel) {
    if(!m_mapModels.contains(strModel)) {
        return LoadGLBModel(strModel, strModel);
    }

    return m_mapModels.at(strModel);
}

ModelAsset_t& CClientAssetManager::GetModel(HModel hModel) {
    if(m_vecModels.size() > hModel) {
        return m_vecModels[hModel];
    }

    throw std::runtime_error(("Missing model asset " + std::to_string(hModel)).c_str());
}

HModel CClientAssetManager::LoadGLBModel(std::string name, std::string file) {
	Model* model = new Model();
	GLBModel* glbModel = GLBFileLoader::LoadUsingLib(file, this);

	for(const auto& skin : glbModel->Skins) {
		Armature* armature = new Armature();
		const auto& glbSkin = skin.second;

		for(int i = 0; i < glbSkin.Joints.size(); i++) {
			const auto& joint = glbSkin.Joints[i];
			Bone bone = Bone();
			bone.Index = joint;
			armature->bones.push_back(bone);
		}

		for(int i = 0; i < glbSkin.Joints.size(); i++) {
			for(auto childIndex : glbModel->Nodes[glbSkin.Joints[i]].Children) {
				for(int j = 0; j < armature->bones.size(); j++) {
					if(armature->bones[j].Index == childIndex) {
						armature->bones[j].parent_index = glbSkin.Joints[i];
					}
				}
			}
		}

		armature->global_inverse_bind_poses = glbSkin.InverseBindMatrices;

		model->Skins.emplace(skin.first, armature);
	}

    for(const auto& glbAnimationEntry : glbModel->Animations) {
        GLBAnimation glbAnimation = glbAnimationEntry.second;
        Animation* animation = new Animation();
        model->Animations.emplace(glbAnimation.Name, animation);

        animation->duration = glbAnimation.Duration;
            
		float fMaxTime = 0;
        for(const auto& channel : glbAnimation.Channels) {
            AnimationTrack track;
			track.Path = channel.Path;
            track.NodeIndex = channel.TargetNode;
            for(const auto& keyFrame : channel.KeyFrames) {
				AnimationKeyFrame kf = AnimationKeyFrame();
                BonePosition bn = BonePosition();
                bn.rotation = keyFrame.Rotation;
                bn.translation = keyFrame.Translation;
				kf.Position = bn;
				kf.Time = keyFrame.Time;
                track.Positions.push_back(kf);

				if(kf.Time > fMaxTime) {
					fMaxTime = kf.Time;
				}
            }
            animation->animation_tracks.push_back(track);
			animation->duration = fMaxTime;
        }
    }

    HModel hModel = m_vecModels.size();

    ModelAsset_t asset {
        .pGlbModel = glbModel,
        .pModel = nullptr
    };

    m_vecModels.push_back(asset);
    m_mapModels.emplace(name, hModel);

    return hModel;
}

pugi::xml_document CClientAssetManager::LoadXMLFile(std::string strFileName) {
    std::vector<uint8_t> mnfData = LoadFile(strFileName);

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_buffer(mnfData.data(), mnfData.size(), 116u, pugi::encoding_utf8);

    if(result.status != pugi::xml_parse_status::status_ok) {
        return {};
    }

    return doc;
}

CClientGameData CClientAssetManager::LoadManifest() {
    CClientGameData gameData{};
    
    for(std::string chrFileName : GetFileNamesByExtension("data", ".chr")) {
        pugi::xml_document doc = LoadXMLFile(chrFileName);
        pugi::xml_node rootNode = doc.child("character_data");
        CCharacterData charData = LoadCharacter(rootNode);
        gameData.mapCharacterData.emplace(charData.strId, charData);
    }

    for(std::string mdlFileName : GetFileNamesByExtension("data", ".mdl")) {
        pugi::xml_document doc = LoadXMLFile(mdlFileName);
        pugi::xml_node rootNode = doc.child("model_data");
        CModelData modelData = LoadModelData(rootNode);
        gameData.mapModelData.emplace(modelData.id, modelData);
    }

    for(std::string ablFileName : GetFileNamesByExtension("data", ".abl")) {
        pugi::xml_document doc = LoadXMLFile(ablFileName);
        pugi::xml_node rootNode = doc.child("ability_data");
        CAbilityData abilityData = LoadAbility(rootNode);
        gameData.mapAbilityData.emplace(abilityData.strId, abilityData);
    }

    return gameData;
}

std::vector<float> CClientAssetManager::ParseFloatVec(std::string str) {
    std::vector<float> result;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, ',')) {
        result.push_back(std::stof(item));
    }
    return result;
}

CAbilityData CClientAssetManager::LoadAbility(pugi::xml_node& abilityDataNode) {
    CAbilityData abilityData{};
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

CModelData CClientAssetManager::LoadModelData(pugi::xml_node& modelNode) {
    CModelData model{};

    std::string modelId = modelNode.attribute("id").as_string();
    std::string modelPath = modelNode.attribute("model").as_string();
    LoadGLBModel(modelId, modelPath);

    pugi::xml_node animationsNode = modelNode.child("animations");

    for(pugi::xml_node animationNode : animationsNode.children()) {
        if(!animationNode || strcmp(animationNode.name(), "animation")) {
            continue;
        }

        CAnimationData animData{};
        animData.name = animationNode.attribute("name").as_string();
        animData.fDuration = animationNode.attribute("duration").as_int();
        model.mapAnimations.emplace(animData.name, animData);
    }
    return model;
}

CCharacterData CClientAssetManager::LoadCharacter(pugi::xml_node& charDataNode) {
    CCharacterData charData{};
    
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

    if(charDataNode.attribute("model")) {
        std::string modelId = charDataNode.attribute("model").as_string();
        charData.modelId = modelId;
    }

    return charData;
}