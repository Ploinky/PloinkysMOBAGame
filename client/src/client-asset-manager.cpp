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


