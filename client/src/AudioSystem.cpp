#include "AudioSystem.h"

#include <combaseapi.h>
#include <string>
#include "Settings.h"
#include <Common/PMG_Common.h>


#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'

bool AudioSystem::Initialize(AssetManager* pAssetManager) {
	HRESULT hr;

	pXAudio2 = nullptr;
	if (FAILED(hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR))) {
		return false;
	}
		
	pMasterVoice = nullptr;
	if (FAILED(hr = pXAudio2->CreateMasteringVoice(&pMasterVoice))) {
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

    LoadSoundFile("UI/Buttons/MenuButton\\MenuButtonClick.wav", pAssetManager);
    LoadSoundFile("characters/stormcaller/abilities\\thunderstrike.wav", pAssetManager);

    // 3D sounds
    if (FAILED(X3DAudioInitialize(0, 33300, m_h3dAudio))) {
        return false;
    }

    return true;
}

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

void AudioSystem::LoadSoundFile(std::string strSoundName, AssetManager* pAssetManager) {
    std::vector<uint8_t> vecFileData = pAssetManager->LoadFile(strSoundName);

    if (vecFileData.size() == 0) {
        Logger::Err("Failed to play sound");
        return;
    }

    DWORD dwChunkSize;
    DWORD dwChunkPosition;
    //check the file type, should be fourccWAVE or 'XWMA'
    FindChunk(vecFileData, fourccRIFF, dwChunkSize, dwChunkPosition);
    DWORD filetype;
    ReadChunkData(vecFileData, &filetype, sizeof(DWORD), dwChunkPosition);
    if (filetype != fourccWAVE) {
        Logger::Err("Failed to play sound");
        return;
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

    Sound* s = new Sound();
    s->Buffer = buffer;
    s->Format = wfx;
    m_mapSounds.emplace(strSoundName, s);
}

void AudioSystem::SetMasterVolume(float value) {
    value = std::min(1.0f, std::max(value, 0.0f));

    pMasterVoice->SetVolume(value);
}

void AudioSystem::Update() {
    // erase voices for sounds that are done playing
    // TODO: use a pool of voices instead of creating and deleting them every time!
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
}

void AudioSystem::StartPlayingSound(std::string strSoundName) {
    std::map<std::string, Sound*>::iterator soundIt = m_mapSounds.find(strSoundName);

    if (soundIt == m_mapSounds.end()) {
        Logger::Err("Attempted to play invalid sound <" + strSoundName + ">");
        return;
    }

    Sound* sound = soundIt->second;

    IXAudio2SourceVoice* pSourceVoice = nullptr;
        
    XAUDIO2_SEND_DESCRIPTOR send{ 0, music_submix_voice_ };
    XAUDIO2_VOICE_SENDS sendList{ 1, &send };
    const XAUDIO2_VOICE_SENDS* sl = &sendList;
    if (FAILED(pXAudio2->CreateSourceVoice(&pSourceVoice, &sound->Format, 0, XAUDIO2_DEFAULT_FREQ_RATIO, 0, sl, NULL))) {
        Logger::Err("Failed to play sound");
        return;
    }
    pSourceVoice->SetOutputVoices(sl);
    if (FAILED(pSourceVoice->SubmitSourceBuffer(&sound->Buffer))) {
        Logger::Err("Failed to play sound");
        return;
    }
    if (FAILED(pSourceVoice->Start(0))) {
        Logger::Err("Failed to play sound");
        return;
    }

    m_vecPlayingSounds.push_back(pSourceVoice);
}
