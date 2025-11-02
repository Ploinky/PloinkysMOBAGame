#pragma once

#include "common/AssetManager.h"
#include <unordered_map>

#include <limits>

#include "GLBFileLoader.h"

class Model;

typedef uint64_t ASSET_HANDLE;
constexpr ASSET_HANDLE INVALID_ASSET_HANDLE = std::numeric_limits<uint64_t>::max();

typedef ASSET_HANDLE HBitmap;
typedef ASSET_HANDLE HTexture;
typedef ASSET_HANDLE HSound;
typedef ASSET_HANDLE HModel;

typedef struct {
    IWICFormatConverter* pConvertedData;
    ID2D1Bitmap* pBitmap;
} BitmapAsset_t;

typedef struct {
    std::vector<uint8_t> data;
    ID3D11ShaderResourceView* pTexture;
    UINT uWidth;
    UINT uHeight;
} TextureAsset_t;

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
    CClientAssetManager();
    
    void Cleanup();

    HBitmap LoadBitmapImage(std::string strBitmap);
    BitmapAsset_t& GetBitmapImage(HBitmap hBitmap);

    HTexture LoadTexture(std::string strTexture);
    HTexture LoadTextureFromData(std::vector<uint8_t> data);
    TextureAsset_t& GetTexture(HTexture hTexture);

    HSound LoadSound(std::string strSound);
    SoundAsset_t& GetSound(HSound hSound);

    HModel LoadModel(std::string strModel);
    ModelAsset_t& GetModel(HModel hModel);

    HModel LoadGLBModel(std::string name, std::string file);
    void LoadCharacterManifest(std::string strCharacterId);

private:
    IWICImagingFactory* m_pWicFactory;

    std::unordered_map<std::string, HBitmap> m_mapBitmaps;
    std::vector<BitmapAsset_t> m_vecBitmaps;

    std::unordered_map<std::string, HTexture> m_mapTextures;
    std::vector<TextureAsset_t> m_vecTextures;

    std::unordered_map<std::string, HSound> m_mapSounds;
    std::vector<SoundAsset_t> m_vecSounds;

    std::unordered_map<std::string, HModel> m_mapModels;
    std::vector<ModelAsset_t> m_vecModels;
    
};