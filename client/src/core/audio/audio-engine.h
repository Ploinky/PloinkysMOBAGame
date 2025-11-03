#pragma once

#include "common/PMG_Common.h"
#include <vector>

// TODO copypasta?
typedef ASSET_HANDLE HVoice;
const HVoice INVALID_VOICE_HANDLE = INVALID_ASSET_HANDLE;

typedef ASSET_HANDLE HListener;
const HListener INVALID_LISTENER_HANDLE = INVALID_ASSET_HANDLE;

typedef ASSET_HANDLE HEmitter;
const HEmitter INVALID_EMITTER_HANDLE = INVALID_ASSET_HANDLE;

typedef ASSET_HANDLE HSound;

class IAudioEngine {
public:
	virtual bool Initialize() = 0;

    virtual void Update() = 0;

    virtual void SetMasterVolume(float fValue) = 0;

	virtual HSound LoadSound(std::vector<uint8_t> vecSoundData) = 0;
    virtual HVoice CreateVoice(HSound sound) = 0;

	virtual HEmitter CreateEmitter(Vector3 vec3InitialPos) = 0;

	virtual void SetListenerPosition(Vector3 vec3NewPos) = 0;

	virtual void UpdateVoicePosition(HVoice hVoice, HEmitter hEmitter, Vector3 vec3Position) = 0;

private:
	Vector3 m_listenerPosition;
};

