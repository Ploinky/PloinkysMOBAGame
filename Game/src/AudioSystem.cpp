#include "AudioSystem.h"

#include <combaseapi.h>
#include <string>
#include "Settings.h"
#include "logger.h"


#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'

namespace PMG {
	bool AudioSystem::Initialize() {
		HRESULT hr;
		hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		if (FAILED(hr)) {
			return false;
		}

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
        return true;
	}

    HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition)
    {
        HRESULT hr = S_OK;
        if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
            return HRESULT_FROM_WIN32(GetLastError());

        DWORD dwChunkType;
        DWORD dwChunkDataSize;
        DWORD dwRIFFDataSize = 0;
        DWORD dwFileType;
        DWORD bytesRead = 0;
        DWORD dwOffset = 0;

        while (hr == S_OK)
        {
            DWORD dwRead;
            if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
                hr = HRESULT_FROM_WIN32(GetLastError());

            if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
                hr = HRESULT_FROM_WIN32(GetLastError());

            switch (dwChunkType)
            {
            case fourccRIFF:
                dwRIFFDataSize = dwChunkDataSize;
                dwChunkDataSize = 4;
                if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
                    hr = HRESULT_FROM_WIN32(GetLastError());
                break;

            default:
                if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
                    return HRESULT_FROM_WIN32(GetLastError());
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

    HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset)
    {
        HRESULT hr = S_OK;
        if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
            return HRESULT_FROM_WIN32(GetLastError());
        DWORD dwRead;
        if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    void AudioSystem::SetMasterVolume(double value) {
        value = min(1, max(value, 0));

        pMasterVoice->SetVolume(value);
    }

    void AudioSystem::Update() {
        std::vector<entity_id> ids_to_remove;

        for (entity_id id : registry.GetEntities<AudioComponent>()) {
            AudioComponent* comp = registry.GetComponent<AudioComponent>(id);

            if (!comp->isPlaying) {
                // Open the file
                HANDLE hFile = CreateFile(
                    comp->fileName.c_str(),
                    GENERIC_READ,
                    FILE_SHARE_READ,
                    NULL,
                    OPEN_EXISTING,
                    0,
                    NULL);

                if (INVALID_HANDLE_VALUE == hFile) {
                    Logger::Err("Failed to play sound");
                    continue;
                }

                if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN)) {
                    Logger::Err("Failed to play sound");
                    continue;
                }

                DWORD dwChunkSize;
                DWORD dwChunkPosition;
                //check the file type, should be fourccWAVE or 'XWMA'
                FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
                DWORD filetype;
                ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
                if (filetype != fourccWAVE) {
                    Logger::Err("Failed to play sound");
                    continue;
                }

                WAVEFORMATEX wfx{};

                FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
                ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);

                //fill out the audio data buffer with the contents of the fourccDATA chunk
                FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
                BYTE* pDataBuffer = new BYTE[dwChunkSize];
                ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);

                XAUDIO2_BUFFER buffer{};
                buffer.AudioBytes = dwChunkSize;  //size of the audio buffer in bytes
                buffer.pAudioData = pDataBuffer;  //buffer containing audio data
                buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer


                XAUDIO2_SEND_DESCRIPTOR send{ 0, music_submix_voice_ };
                XAUDIO2_VOICE_SENDS sendList{ 1, &send };
                const XAUDIO2_VOICE_SENDS* sl = &sendList;
                if (FAILED(pXAudio2->CreateSourceVoice(&pSourceVoice, ((WAVEFORMATEX*)&wfx), 0, XAUDIO2_DEFAULT_FREQ_RATIO, 0, sl, NULL))) {
                    Logger::Err("Failed to play sound");
                    continue;
                }
                pSourceVoice->SetOutputVoices(sl);
                if (FAILED(pSourceVoice->SubmitSourceBuffer(&buffer))) {
                    Logger::Err("Failed to play sound");
                    continue;
                }
                if (FAILED(pSourceVoice->Start(0))) {
                    Logger::Err("Failed to play sound");
                    continue;
                }

                comp->pSourceVoice = pSourceVoice;
                comp->isPlaying = true;
                comp->shouldStopPlaying = false;

                return;
            }

            if (comp->shouldStopPlaying) {
                comp->pSourceVoice->Stop();
                comp->pSourceVoice->DestroyVoice();
                ids_to_remove.push_back(id);
            }
        }

        for (entity_id id_to_remove : ids_to_remove) {
            registry.RemoveComponent<AudioComponent>(id_to_remove);
        }
    }
}