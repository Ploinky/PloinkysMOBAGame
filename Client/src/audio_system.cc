#include "audio_system.h"

#include <combaseapi.h>
#include <string>
#include "settings.h"

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

        return true;
	}

    void AudioSystem::Update() {
        float val = 0;
        pMasterVoice->GetVolume(&val);
        if (fabs(Settings::GetDouble(PMGSettings::MASTER_VOLUME) - val) >= 0.001f) {
            HRESULT hr = pSourceVoice->SetVolume(Settings::GetDouble(PMGSettings::MASTER_VOLUME));
            pXAudio2->CommitChanges(XAUDIO2_COMMIT_ALL);
            if (FAILED(hr)) {

            }

        }
    }

    #define fourccRIFF 'FFIR'
    #define fourccDATA 'atad'
    #define fourccFMT ' tmf'
    #define fourccWAVE 'EVAW'
    #define fourccXWMA 'AMWX'
    #define fourccDPDS 'sdpd'

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

    bool AudioSystem::StartPlayingSound() {
        std::wstring strFileName = TEXT("audio.wav");

        // Open the file
        HANDLE hFile = CreateFile(
            strFileName.c_str(),
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);

        if (INVALID_HANDLE_VALUE == hFile)
            return false;

        if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
            return HRESULT_FROM_WIN32(GetLastError());

        DWORD dwChunkSize;
        DWORD dwChunkPosition;
        //check the file type, should be fourccWAVE or 'XWMA'
        FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
        DWORD filetype;
        ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
        if (filetype != fourccWAVE)
            return S_FALSE;

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

        if (FAILED(pXAudio2->CreateSourceVoice(&pSourceVoice, (WAVEFORMATEX*)&wfx))) {
            return false;
        }

        if (FAILED(pSourceVoice->SubmitSourceBuffer(&buffer))) {
            return false;
        }
        if (FAILED(pSourceVoice->Start(0))) {
            return false;
        }

        return true;
    }
}