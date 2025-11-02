#include "client-asset-manager.h"

#include "common/PMG_Common.h"
#include "Model.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define CLEANUP(res) if(res != nullptr) { res->Release(); res = nullptr;}

CClientAssetManager::CClientAssetManager(IGraphicsEngine* pGraphicsEngine) {
    m_pGraphicsEngine = pGraphicsEngine;
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
        throw std::exception(("Bitmap asset <" + strBitmap + "> missing").c_str());
    }

    m_mapBitmaps.emplace(strBitmap, hBitmap);
    return hBitmap;
}


HTexture CClientAssetManager::LoadTexture(std::string strTexture) {
    if(m_mapTextures.contains(strTexture)) {
        return m_mapTextures.at(strTexture);
    }

    int width, height, channels_in_file;
    std::vector<uint8_t> imageData = LoadFile(strTexture);
    unsigned char* pImageData = stbi_load_from_memory(
        imageData.data(),
        imageData.size(),
        &width,
        &height,
        &channels_in_file,
        4
    );

    HTexture hTexture = m_pGraphicsEngine->LoadTexture(pImageData, width, height);

    m_mapTextures.emplace(strTexture, hTexture);
    return hTexture;
}

#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'

HRESULT FindChunk(std::vector<uint8_t>& vecFileData, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition) {
    DWORD dwChunkType;
    DWORD dwChunkDataSize;
    DWORD dwRIFFDataSize = 0;
    DWORD dwFileType;
    DWORD bytesRead = 0;
    DWORD dwOffset = 0;
    DWORD dwFilePosition = 0;

    while (dwOffset <= vecFileData.size()) {
        DWORD dwRead;
        std::memcpy(&dwChunkType, vecFileData.data() + dwFilePosition, sizeof(dwChunkType));
        dwFilePosition += sizeof(dwChunkType);
        std::memcpy(&dwChunkDataSize, vecFileData.data() + dwFilePosition, sizeof(dwChunkType));
        dwFilePosition += sizeof(dwChunkDataSize);
    
        switch (dwChunkType)
        {
            case fourccRIFF:
                dwRIFFDataSize = dwChunkDataSize;
                dwChunkDataSize = 4;
                std::memcpy(&dwFileType, vecFileData.data() + dwFilePosition, sizeof(dwFileType));
                dwFilePosition += sizeof(dwFileType);
                break;

            default:
                dwFilePosition += dwChunkDataSize;
                break;
        }

        dwOffset += sizeof(DWORD) * 2;

        if (dwChunkType == fourcc)
        {
            dwChunkSize = dwChunkDataSize;
            dwChunkDataPosition = dwOffset;
            return S_OK;
        }

        dwOffset += dwChunkDataSize;

        if (bytesRead >= dwRIFFDataSize) return S_FALSE;

    }

    return S_OK;
}

HRESULT ReadChunkData(std::vector<uint8_t>& vecFileData, void* buffer, DWORD buffersize, DWORD bufferoffset)
{
    HRESULT hr = S_OK;
    std::memcpy(buffer, vecFileData.data() + bufferoffset, buffersize);

    return hr;
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

    DWORD dwChunkSize;
    DWORD dwChunkPosition;
    //check the file type, should be fourccWAVE or 'XWMA'
    FindChunk(vecFileData, fourccRIFF, dwChunkSize, dwChunkPosition);
    DWORD filetype;
    ReadChunkData(vecFileData, &filetype, sizeof(DWORD), dwChunkPosition);
    if (filetype != fourccWAVE) {
        Logger::Err("Failed to load sound");
        return INVALID_ASSET_HANDLE;
    }

    WAVEFORMATEX wfx{};

    FindChunk(vecFileData, fourccFMT, dwChunkSize, dwChunkPosition);
    ReadChunkData(vecFileData, &wfx, dwChunkSize, dwChunkPosition);

    //fill out the audio data buffer with the contents of the fourccDATA chunk
    FindChunk(vecFileData, fourccDATA, dwChunkSize, dwChunkPosition);
    BYTE* pDataBuffer = new BYTE[dwChunkSize];
    ReadChunkData(vecFileData, pDataBuffer, dwChunkSize, dwChunkPosition);

    XAUDIO2_BUFFER buffer{};
    buffer.AudioBytes = dwChunkSize;  //size of the audio buffer in bytes
    buffer.pAudioData = pDataBuffer;  //buffer containing audio data
    buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer
    buffer.LoopCount = XAUDIO2_NO_LOOP_REGION;

    SoundAsset_t soundAsset = SoundAsset_t();
    soundAsset.buffer = buffer;
    soundAsset.format = wfx;

    m_vecSounds.push_back(soundAsset);

    HSound handle = m_vecSounds.size() - 1;
    m_mapSounds.emplace(strSound, handle);

    return handle;
}

SoundAsset_t& CClientAssetManager::GetSound(HSound hSound) {
    if(m_vecSounds.size() > hSound) {
        return m_vecSounds[hSound];
    }

    throw std::exception(("Missing sound asset " + std::to_string(hSound)).c_str());
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

    throw std::exception(("Missing model asset " + std::to_string(hModel)).c_str());
}

HModel CClientAssetManager::LoadGLBModel(std::string name, std::string file) {
	Model* model = new Model();
	GLBModel* glbModel = GLBFileLoader::LoadModelFromGLBFile(file, this);

	for(const auto& skin : glbModel->Skins) {
		Armature* armature = new Armature();
		const auto& glbSkin = skin.second;

		for(int i = 0; i < glbSkin->Joints.size(); i++) {
			const auto& joint = glbSkin->Joints[i];
			Bone bone = Bone();
			bone.Index = joint;
			armature->bones.push_back(bone);
		}

		for(int i = 0; i < glbSkin->Joints.size(); i++) {
			for(auto childIndex : glbModel->Nodes[glbSkin->Joints[i]]->Children) {
				for(int j = 0; j < armature->bones.size(); j++) {
					if(armature->bones[j].Index == childIndex) {
						armature->bones[j].parent_index = glbSkin->Joints[i];
					}
				}
			}
		}

		armature->global_inverse_bind_poses = glbSkin->InverseBindMatrices;

		model->Skins.emplace(skin.first, armature);
	}

    for(const auto& glbAnimationEntry : glbModel->Animations) {
        GLBAnimation* glbAnimation = glbAnimationEntry.second;
        Animation* animation = new Animation();
        model->Animations.emplace(glbAnimation->Name, animation);

        animation->duration = glbAnimation->Duration;
            
		float fMaxTime = 0;
        for(const auto& channel : glbAnimation->Channels) {
            AnimationTrack track;
			track.Path = channel->Path;
            track.NodeIndex = channel->TargetNode;
            for(const auto& keyFrame : channel->KeyFrames) {
				AnimationKeyFrame kf = AnimationKeyFrame();
                BonePosition bn = BonePosition();
                bn.rotation = DirectX::XMLoadFloat4(&keyFrame.Rotation);
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
		std::string((char*) LoadFile("characters/" + strCharacterId + "/character_manifest.json").data())
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