#pragma once

#include "Common/AssetManager.h"
#include <unordered_map>

#include <limits>
#include <d2d1.h>
#include <d3d11.h>
#include "wincodec.h"

typedef uint64_t ASSET_HANDLE;
constexpr ASSET_HANDLE INVALID_ASSET_HANDLE = std::numeric_limits<uint64_t>::max();

typedef ASSET_HANDLE HBitmap;
typedef ASSET_HANDLE HTexture;

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

class CClientAssetManager : public AssetManager {
public:
    CClientAssetManager();
    
    void Cleanup();

    HBitmap LoadBitmapImage(std::string strBitmap);
    BitmapAsset_t& GetBitmapImage(HBitmap hBitmap);

    HTexture LoadTexture(std::string strTexture);
    HTexture LoadTextureFromData(std::vector<uint8_t> data);
    TextureAsset_t& GetTexture(HTexture hTexture);

private:
    IWICImagingFactory* m_pWicFactory;

    std::unordered_map<std::string, HBitmap> m_mapBitmaps;
    std::vector<BitmapAsset_t> m_vecBitmaps;

    std::unordered_map<std::string, HTexture> m_mapTextures;
    std::vector<TextureAsset_t> m_vecTextures;
};