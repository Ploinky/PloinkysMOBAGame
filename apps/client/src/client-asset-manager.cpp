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
	GLBModel* glbModel = GLBFileLoader::LoadModelFromGLBFile(file, this);

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

void CClientAssetManager::LoadCharacterManifest(std::string strCharacterId) {
	PJL::JSONValue manifestValue = PJL::JSONParser().Parse(
		std::string((char*) LoadFile("data/characters/" + strCharacterId + "/character_manifest.json").data())
	);

	if(!manifestValue.IsObject()) {
		Logger::Err("Failed to load manifest for character " + strCharacterId);
		return;
	}

	PJL::JSONObject manifest = manifestValue.AsObject();

	if(manifest.Contains("model") && manifest.Get("model").IsString()) {
		LoadGLBModel(strCharacterId, manifest.Get("model").AsString());
	}

	if(manifest.Contains("icons") && manifest.Get("icons").IsArray()) {
		PJL::JSONArray arrIcons = manifest.Get("icons").AsArray();
		for(int i = 0; i < arrIcons.Size(); i++) {
			PJL::JSONValue val = arrIcons.Get(i);

			if(val.IsString()) {
				// TODO this moves to asset manager
				// bitmaps_.emplace(val.AsString(), CreateBitmapFromData(pAssetManager->LoadFile(val.AsString())));
			}
		}
	}
}