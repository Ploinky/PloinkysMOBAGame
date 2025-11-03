#include "audio-engine-xaudio.h"

#include "client-asset-manager.h"
#include "common/PMG_Common.h"

bool CAudioEngineXAudio::Initialize() {
	HRESULT hr;

	pXAudio2 = nullptr;
	if (FAILED(hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR))) {
		return false;
	}

    XAUDIO2_DEBUG_CONFIGURATION debugConf = {};
    debugConf.TraceMask = XAUDIO2_LOG_ERRORS ;
    pXAudio2->SetDebugConfiguration(&debugConf, nullptr);

	pMasterVoice = nullptr;
	if (FAILED(hr = pXAudio2->CreateMasteringVoice(&pMasterVoice, 2))) {
		return false;
	}

    pMasterVoice->SetVolume(1);

    music_submix_voice_ = nullptr;
    if (FAILED(hr = pXAudio2->CreateSubmixVoice(&music_submix_voice_, 1, 44100, 0, 0, 0, 0))) {
        return false;
    }

    music_submix_voice_->SetVolume(1);
    SFXSend = { 0, music_submix_voice_ };
    SFXSendList = { 1, &SFXSend };

    if (FAILED(X3DAudioInitialize(SPEAKER_STEREO, 33300, m_h3dAudio))) {
        return false;
    }

    m_listener = {};
    m_listener.Position = X3DAUDIO_VECTOR({0, 0, 0});
    m_listener.OrientTop = X3DAUDIO_VECTOR({0, 1, 0});
    m_listener.OrientFront = X3DAUDIO_VECTOR({0.0f, -0.866f, 0.5f}); // magic rts camera forward vector for z- -> forward
    m_listenerCone = {};
    m_listenerCone.InnerAngle = ToRadians(60);
    m_listenerCone.OuterAngle = ToRadians(90);
    m_listenerCone.InnerVolume = 1.0f;
    m_listenerCone.OuterVolume = 0.3f;
    m_listener.pCone = &m_listenerCone;

    return true;
}

void CAudioEngineXAudio::Update() {
    // erase voices for sounds that are done playing
    // TODO: use a pool of voices instead of creating and deleting them every time!
    // TODO: actually delete the voices eventually :O
    /*
    std::erase_if(m_vecPlayingSounds, [](IXAudio2SourceVoice* sound) {
        XAUDIO2_VOICE_STATE state;
        sound->GetState(&state);
        
        if (state.BuffersQueued == 0) {
            sound->Stop();
            sound->DestroyVoice();
            return true;
            
        }
        
        return false;
    });
    */
}


void CAudioEngineXAudio::SetMasterVolume(float fValue) {
    fValue = std::min(1.0f, std::max(fValue, 0.0f));

    pMasterVoice->SetVolume(fValue);
}

HVoice CAudioEngineXAudio::CreateVoice(HSound hSound) {
    if(hSound == INVALID_ASSET_HANDLE || hSound > m_vecSounds.size()) {
        return INVALID_VOICE_HANDLE;
    }

    SoundAsset_t sound = m_vecSounds.at(hSound);

    IXAudio2SourceVoice* pSourceVoice = nullptr;

    XAUDIO2_SEND_DESCRIPTOR send{ 0, pMasterVoice };
    XAUDIO2_VOICE_SENDS sendList{ 1, &send };
    const XAUDIO2_VOICE_SENDS* sl = &sendList;
    if (FAILED(pXAudio2->CreateSourceVoice(&pSourceVoice, &sound.format, 0, XAUDIO2_DEFAULT_FREQ_RATIO, 0, sl, NULL))) {
        Logger::Err("Failed to play sound");
        return INVALID_VOICE_HANDLE;
    }
    pSourceVoice->SetOutputVoices(sl);
    if (FAILED(pSourceVoice->SubmitSourceBuffer(&sound.buffer))) {
        Logger::Err("Failed to play sound");
        return INVALID_VOICE_HANDLE;
    }
    if (FAILED(pSourceVoice->Start(0))) {
        Logger::Err("Failed to play sound");
        return INVALID_VOICE_HANDLE;
    }

    m_vecPlayingSounds.push_back(pSourceVoice);
    return m_vecPlayingSounds.size() - 1;
}

HEmitter CAudioEngineXAudio::CreateEmitter(Vector3 vec3InitialPos) {
    X3DAUDIO_EMITTER emitter = {};
    emitter.Position = X3DAUDIO_VECTOR({vec3InitialPos.x, vec3InitialPos.y, -vec3InitialPos.z});
    emitter.ChannelCount = 1;
    
    m_vecEmitters.push_back(emitter);

    return m_vecEmitters.size() - 1;
}

void CAudioEngineXAudio::SetListenerPosition(Vector3 vec3NewPos) {
    Vector3 scaledPos = vec3NewPos.ScaleToLength(vec3NewPos.Length());
    m_listener.Position = X3DAUDIO_VECTOR({scaledPos.x, scaledPos.y, -scaledPos.z});
}

void CAudioEngineXAudio::UpdateVoicePosition(HVoice hVoice, HEmitter hEmitter, Vector3 vec3Position) {
    if(hVoice == INVALID_VOICE_HANDLE) {
        Logger::FormatErr("failed to update void - invalid handle: %lu", hVoice);
        return;
    }

    X3DAUDIO_EMITTER& emitter = m_vecEmitters.at(hEmitter);
    Vector3 scaledPos = vec3Position.ScaleToLength(vec3Position.Length());
    emitter.Position = X3DAUDIO_VECTOR({scaledPos.x, scaledPos.y, -scaledPos.z});
    emitter.ChannelCount = 1;
    emitter.CurveDistanceScaler = 3000;
    emitter.pVolumeCurve = &volumeCurve;

    IXAudio2SourceVoice* pVoice = m_vecPlayingSounds.at(hVoice);
    XAUDIO2_VOICE_DETAILS sourceDetails;
    pVoice->GetVoiceDetails(&sourceDetails);

    XAUDIO2_VOICE_DETAILS masterDetails;
    pMasterVoice->GetVoiceDetails(&masterDetails);

    X3DAUDIO_DSP_SETTINGS DSPSettings = {};
    FLOAT32 * matrix = new FLOAT32[masterDetails.InputChannels];
    DSPSettings.SrcChannelCount = 1;
    DSPSettings.DstChannelCount = masterDetails.InputChannels;
    DSPSettings.pMatrixCoefficients = matrix;

    X3DAudioCalculate(m_h3dAudio, &m_listener, &emitter,
    X3DAUDIO_CALCULATE_MATRIX,
    &DSPSettings);
    
    HRESULT hr = pVoice->SetOutputMatrix(pMasterVoice, 1, masterDetails.InputChannels, DSPSettings.pMatrixCoefficients);
    if(FAILED(hr)) {
        throw std::runtime_error("FAILED TO UPDATE OUTPUT MATRIX");
    }

    delete[] matrix;
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

HSound CAudioEngineXAudio::LoadSound(std::vector<uint8_t> vecSoundData) {
    DWORD dwChunkSize;
    DWORD dwChunkPosition;
    //check the file type, should be fourccWAVE or 'XWMA'
    FindChunk(vecSoundData, fourccRIFF, dwChunkSize, dwChunkPosition);
    DWORD filetype;
    ReadChunkData(vecSoundData, &filetype, sizeof(DWORD), dwChunkPosition);
    if (filetype != fourccWAVE) {
        Logger::Err("Failed to load sound");
        return INVALID_ASSET_HANDLE;
    }

    WAVEFORMATEX wfx{};

    FindChunk(vecSoundData, fourccFMT, dwChunkSize, dwChunkPosition);
    ReadChunkData(vecSoundData, &wfx, dwChunkSize, dwChunkPosition);

    //fill out the audio data buffer with the contents of the fourccDATA chunk
    FindChunk(vecSoundData, fourccDATA, dwChunkSize, dwChunkPosition);
    BYTE* pDataBuffer = new BYTE[dwChunkSize];
    ReadChunkData(vecSoundData, pDataBuffer, dwChunkSize, dwChunkPosition);

    XAUDIO2_BUFFER buffer{};
    buffer.AudioBytes = dwChunkSize;  //size of the audio buffer in bytes
    buffer.pAudioData = pDataBuffer;  //buffer containing audio data
    buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer
    buffer.LoopCount = XAUDIO2_NO_LOOP_REGION;

    SoundAsset_t soundAsset = SoundAsset_t();
    soundAsset.buffer = buffer;
    soundAsset.format = wfx;

    m_vecSounds.push_back(soundAsset);

    return m_vecSounds.size() - 1;
}