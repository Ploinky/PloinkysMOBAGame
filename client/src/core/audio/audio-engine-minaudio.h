#pragma once

#include <core/audio/audio-engine.h>
#include <vendor/minaudio.h>

class CAudioEngineMinaudio : public IAudioEngine {
public:
	virtual bool Initialize() override;

    virtual void Update() override;

    virtual void SetMasterVolume(float fValue) override;

	virtual HSound LoadSound(std::vector<uint8_t> vecSoundData) override;
    virtual HVoice CreateVoice(HSound sound) override;

	virtual HEmitter CreateEmitter(Vector3 vec3InitialPos) override;

	virtual void SetListenerPosition(Vector3 vec3NewPos) override;

	virtual void UpdateVoicePosition(HVoice hVoice, HEmitter hEmitter, Vector3 vec3Position) override;
};