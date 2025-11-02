#pragma once

#include "common/AssetManager.h"
#include <unordered_map>

#include <limits>

#include "GLBFileLoader.h"
#include <core/graphics/graphics-engine.h>

class Model;

typedef struct {
    XAUDIO2_BUFFER buffer;
    WAVEFORMATEX format;
} SoundAsset_t;

typedef struct { 
    GLBModel* pGlbModel;
    Model* pModel;
} ModelAsset_t;

class CClientAssetManager : public AssetManager {
public:
    CClientAssetManager(IGraphicsEngine* pGraphicsEngine);
    
    void Cleanup();

    HBitmap GetBitmapImage(std::string strBitmap);

    HTexture LoadTexture(std::string strTexture);

    HSound LoadSound(std::string strSound);
    SoundAsset_t& GetSound(HSound hSound);

    HModel LoadModel(std::string strModel);
    ModelAsset_t& GetModel(HModel hModel);

    HModel LoadGLBModel(std::string name, std::string file);
    void LoadCharacterManifest(std::string strCharacterId);

private:
    IGraphicsEngine* m_pGraphicsEngine;

    std::unordered_map<std::string, HBitmap> m_mapBitmaps;

    std::unordered_map<std::string, HTexture> m_mapTextures;

    std::unordered_map<std::string, HSound> m_mapSounds;
    std::vector<SoundAsset_t> m_vecSounds;

    std::unordered_map<std::string, HModel> m_mapModels;
    std::vector<ModelAsset_t> m_vecModels;
    
};