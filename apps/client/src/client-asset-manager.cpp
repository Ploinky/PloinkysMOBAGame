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

HBitmap CClientAssetManager::GetBitmapImage(std::string id) {
    if(m_mapBitmaps.contains(id)) {
        return m_mapBitmaps.at(id);
    }

    return INVALID_ASSET_HANDLE;
}

HBitmap CClientAssetManager::GetBitmapImage(std::string id, std::string strBitmap) {
    if(m_mapBitmaps.contains(id)) {
        return m_mapBitmaps.at(id);
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
        m_mapBitmaps.emplace(id, hBitmap);
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
HSound CClientAssetManager::LoadSound(std::string strId, std::string strSound) {
    if(m_mapSounds.contains(strId)) {
        return m_mapSounds[strId];
    }
    
    std::vector<uint8_t> vecFileData = LoadFile(strSound);

    if (vecFileData.size() == 0) {
        Logger::Err("Failed to load sound");
        return INVALID_ASSET_HANDLE;
    }

    HSound hSound = m_pAudioEngine->LoadSound(vecFileData);
    m_mapSounds.emplace(strId, hSound);
    return hSound;
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

const CGameData& CClientAssetManager::GetGameData() {
    return m_gameData;
}

const CGameData& CClientAssetManager::LoadManifest() {
    
    for(std::string xmlFileName : GetFileNamesByExtension("data", ".xml")) {
        pugi::xml_document doc = LoadXMLFile(xmlFileName);
        pugi::xml_node entityNode = doc.child("entity");

        if(entityNode) {
            CCharacterData charData = LoadCharacter(entityNode);
            m_gameData.mapCharacterData.emplace(charData.strId, charData);
            Logger::FormatMsg("Loaded entity %s from %s", charData.strId.c_str(), xmlFileName.c_str());
            continue;
        }

        pugi::xml_node modelNode = doc.child("model_data");
        if(modelNode) {
            CModelData modelData = LoadModelData(modelNode);
            m_gameData.mapModelData.emplace(modelData.id, modelData);
            Logger::FormatMsg("Loaded model %s from %s", modelData.id.c_str(), xmlFileName.c_str());
            continue;
        }

        pugi::xml_node abilityNode = doc.child("ability");
        if(abilityNode) {
            CAbilityData abilityData = LoadAbility(abilityNode);
            m_gameData.mapAbilityData.emplace(abilityData.strId, abilityData);
            Logger::FormatMsg("Loaded ability %s from %s", abilityData.strId.c_str(), xmlFileName.c_str());
            continue;
        }

        pugi::xml_node iconNode = doc.child("icon");
        if(iconNode) {
            CIconData iconData = LoadIconData(iconNode);
            m_gameData.mapIconData.emplace(iconData.id, iconData);
            Logger::FormatMsg("Loaded icon %s from %s", iconData.id.c_str(), xmlFileName.c_str());
            continue;
        }

        pugi::xml_node audioNode = doc.child("audio");
        if(audioNode) {
            CAudioAssetData audioData = LoadAudioAssetData(audioNode);
            m_gameData.mapAudioData.emplace(audioData.id, audioData);
            Logger::FormatMsg("Loaded audio asset %s from %s", audioData.id.c_str(), xmlFileName.c_str());
            continue;
        }

        pugi::xml_node mapNode = doc.child("map");
        if(mapNode) {
            CMapData mapData = LoadMapData(mapNode);
            m_gameData.mapMapData.emplace(mapData.strId, mapData);
        }
    }

    return GetGameData();
}

CIconData CClientAssetManager::LoadIconData(pugi::xml_node& iconNode) {
    CIconData iconData{};
    std::string iconId = iconNode.attribute("id").as_string();
    std::string iconPath = iconNode.attribute("asset").as_string();
    GetBitmapImage(iconId, iconPath);
    return iconData;
}

CAudioAssetData CClientAssetManager::LoadAudioAssetData(pugi::xml_node& audioNode) {
    CAudioAssetData audioData{};
    std::string audioId = audioNode.attribute("id").as_string();
    std::string audioPath = audioNode.attribute("asset").as_string();
    LoadSound(audioId, audioPath);
    audioData.id = audioId;
    return audioData;
}

CModelData CClientAssetManager::LoadModelData(pugi::xml_node& modelNode) {
    CModelData model{};

    std::string modelId = modelNode.attribute("id").as_string();
    std::string modelPath = modelNode.attribute("model").as_string();
    LoadGLBModel(modelId, modelPath);

    model.id = modelId;

    pugi::xml_node animationsNode = modelNode.child("animations");

    for(pugi::xml_node animationNode : animationsNode.children()) {
        if(!animationNode || strcmp(animationNode.name(), "animation")) {
            continue;
        }

        CAnimationData animData{};
        animData.name = animationNode.attribute("name").as_string();
        animData.fDuration = animationNode.attribute("duration").as_int();
        std::string slot = animationNode.attribute("slot").as_string();
        model.mapAnimations.emplace(slot, animData);
    }
    return model;
}
