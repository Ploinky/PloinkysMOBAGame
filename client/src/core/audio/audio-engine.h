#pragma once

#include <xaudio2.h>
#include <x3daudio.h>
#include "client-asset-manager.h"
#include "common/pmg_common.h"
#include <vector>

// TODO copypasta?
typedef ASSET_HANDLE HVoice;
const HVoice INVALID_VOICE_HANDLE = INVALID_ASSET_HANDLE;

typedef ASSET_HANDLE HListener;
const HListener INVALID_LISTENER_HANDLE = INVALID_ASSET_HANDLE;

typedef ASSET_HANDLE HEmitter;
const HEmitter INVALID_EMITTER_HANDLE = INVALID_ASSET_HANDLE;

class CAudioEngine {
public:
	bool Initialize(CClientAssetManager* pAssetManager);

    void Update();

    void SetMasterVolume(float fValue);

    HVoice CreateVoice(SoundAsset_t sound);

	HEmitter CreateEmitter(Vector3 vec3InitialPos);

	void SetListenerPosition(Vector3 vec3NewPos);

	void UpdateVoicePosition(HVoice hVoice, HEmitter hEmitter, Vector3 vec3Position);

private:
    CClientAssetManager* m_pAssetManager;

	X3DAUDIO_HANDLE m_h3dAudio;
	IXAudio2* pXAudio2;
	IXAudio2MasteringVoice* pMasterVoice;

	IXAudio2SubmixVoice* music_submix_voice_;

	XAUDIO2_SEND_DESCRIPTOR SFXSend{};
	XAUDIO2_VOICE_SENDS SFXSendList{};
    
	std::vector<IXAudio2SourceVoice*> m_vecPlayingSounds;
	
	X3DAUDIO_CONE m_listenerCone;
	X3DAUDIO_LISTENER m_listener;
	std::vector<X3DAUDIO_EMITTER> m_vecEmitters;

	Vector3 m_listenerPosition;
};

