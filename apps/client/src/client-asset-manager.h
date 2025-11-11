#pragma once

#include "common/AssetManager.h"
#include <unordered_map>

#include <limits>

#include "GLBFileLoader.h"
#include <core/graphics/graphics-engine.h>
#include <core/audio/audio-engine.h>

class Model;

typedef struct { 
    GLBModel* pGlbModel;
    Model* pModel;
} ModelAsset_t;

class CClientAssetManager : public AssetManager {
public:
    CClientAssetManager(IGraphicsEngine* pGraphicsEngine, IAudioEngine* pAudioEngine);

    void Cleanup();

    HBitmap GetBitmapImage(std::string strBitmap);

    HTexture LoadTexture(std::string strTexture);
    HTexture LoadTextureFromData(std::vector<uint8_t> imageData);

    HSound LoadSound(std::string strSound);

    HModel LoadModel(std::string strModel);
    ModelAsset_t& GetModel(HModel hModel);

    HModel LoadGLBModel(std::string name, std::string file);
    void LoadCharacterManifest(std::string strCharacterId);

private:
    IGraphicsEngine* m_pGraphicsEngine;
    IAudioEngine* m_pAudioEngine;

    std::unordered_map<std::string, HBitmap> m_mapBitmaps;

    std::unordered_map<std::string, HTexture> m_mapTextures;

    std::unordered_map<std::string, HSound> m_mapSounds;

    std::unordered_map<std::string, HModel> m_mapModels;
    std::vector<ModelAsset_t> m_vecModels;
    
};