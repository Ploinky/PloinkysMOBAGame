#include "client-asset-manager.h"

#include "wincodec.h"
#include "common/PMG_Common.h"

#define CLEANUP(res) if(res != nullptr) { res->Release(); res = nullptr;}

CClientAssetManager::CClientAssetManager() {
    // Create the COM imaging factory
    HRESULT hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&m_pWicFactory)
    );
}


void CClientAssetManager::Cleanup() {
#ifdef _DEBUG
    Logger::Msg("===============================");
    Logger::Msg("==== Cleaning up assets, count:");
    Logger::FormatMsg("bitmaps: %d", m_vecBitmaps.size());
    Logger::FormatMsg("textures: %d", m_vecTextures.size());
    Logger::Msg("===============================");
#endif

    for(BitmapAsset_t& asset : m_vecBitmaps) {
        CLEANUP(asset.pBitmap)
        CLEANUP(asset.pConvertedData)
    }

    for(TextureAsset_t& asset : m_vecTextures) {
        CLEANUP(asset.pTexture)
    }

    CLEANUP(m_pWicFactory)
}

HBitmap CClientAssetManager::LoadBitmapImage(std::string strBitmap) {
    BitmapAsset_t bitmapAsset = BitmapAsset_t {
        .pConvertedData = nullptr,
        .pBitmap = nullptr,
    };

    if(m_mapBitmaps.contains(strBitmap)) {
        return m_mapBitmaps.at(strBitmap);
    }

    std::vector<uint8_t> data = LoadFile(strBitmap);
    IWICBitmapDecoder* pDecoder = NULL;
    IWICBitmapFrameDecode* pSource = NULL;

    IStream* stream = nullptr;
    HRESULT hr = CreateStreamOnHGlobal(nullptr, TRUE, &stream);

    if (FAILED(hr)) {
        return INVALID_ASSET_HANDLE;
    }

    ULONG bytesWritten = 0;
    hr = stream->Write(data.data(), static_cast<ULONG>(data.size()), &bytesWritten);

    if (FAILED(hr) || bytesWritten != data.size()) {
        return INVALID_ASSET_HANDLE;
    }

    hr = m_pWicFactory->CreateDecoderFromStream(
        stream,
        nullptr,
        WICDecodeMetadataCacheOnLoad,
        &pDecoder
    );
    if (FAILED(hr))
    {
        return INVALID_ASSET_HANDLE;
    }
        // Create the initial frame.
        hr = pDecoder->GetFrame(0, &pSource);

    if (SUCCEEDED(hr))
    {
        // Convert the image format to 32bppPBGRA
        // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
        hr = m_pWicFactory->CreateFormatConverter(&bitmapAsset.pConvertedData);

    }

    if (SUCCEEDED(hr))
    {
        hr = bitmapAsset.pConvertedData->Initialize(
            pSource,
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            NULL,
            0.f,
            WICBitmapPaletteTypeMedianCut
        );
    }

    pDecoder->Release();
    pSource->Release();

    m_vecBitmaps.push_back(bitmapAsset);

    return m_vecBitmaps.size() - 1;
}

BitmapAsset_t& CClientAssetManager::GetBitmapImage(HBitmap hBitmap) {
    if(m_vecBitmaps.size() > hBitmap) {
        return m_vecBitmaps[hBitmap];
    }

    throw std::exception(("Bitmap asset " + std::to_string(hBitmap) + " missing").c_str());
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
HTexture CClientAssetManager::LoadTextureFromData(std::vector<uint8_t> imageData) {
	IWICBitmap* wicBitmap = 0;
	IWICStream* stream = 0;

	if(FAILED(m_pWicFactory->CreateStream(&stream))) {
		return INVALID_ASSET_HANDLE;
	}

	if(FAILED(stream->InitializeFromMemory(imageData.data(), imageData.size()))) {
		return INVALID_ASSET_HANDLE;
	}

	IWICBitmapDecoder* decoder;
	if(FAILED(m_pWicFactory->CreateDecoderFromStream(stream, nullptr, WICDecodeMetadataCacheOnLoad, &decoder))) {
		return INVALID_ASSET_HANDLE;
	}

	IWICBitmapFrameDecode* frame;
	if(FAILED(decoder->GetFrame(0, &frame))) {
		return INVALID_ASSET_HANDLE;
	}

	m_pWicFactory->CreateBitmapFromSource(frame, WICBitmapCacheOnLoad, &wicBitmap);

	UINT width, height;
	wicBitmap->GetSize(&width, &height);

	WICPixelFormatGUID pixelFormat;
	wicBitmap->GetPixelFormat(&pixelFormat);

	WICPixelFormatGUID convertFormat = GUID_WICPixelFormat32bppRGBA;
	IWICBitmapSource* convertedBitmapSource = nullptr;

	if(pixelFormat != convertFormat) {
		IWICFormatConverter* converter;
		m_pWicFactory->CreateFormatConverter(&converter);
		converter->Initialize(wicBitmap, convertFormat, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom);
		convertedBitmapSource = converter;
	} else {
		convertedBitmapSource = wicBitmap;
	}

	// Copy pixel data
	std::vector<uint8_t> pixels(width * height * 4);
	convertedBitmapSource->CopyPixels(nullptr, width * 4, pixels.size(), pixels.data());

    m_vecTextures.push_back(TextureAsset_t{.data = pixels, .pTexture = nullptr, .uWidth = width, .uHeight = height});
    return m_vecTextures.size() - 1;
}

TextureAsset_t& CClientAssetManager::GetTexture(HTexture hTexture) {
    if(m_vecTextures.size() > hTexture) {
        return m_vecTextures[hTexture];
    }

    throw std::exception(("Texture asset " + std::to_string(hTexture) + " missing").c_str());
}