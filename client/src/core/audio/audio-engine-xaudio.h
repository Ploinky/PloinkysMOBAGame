#include "audio-engine.h"

#include <xaudio2.h>
#include <x3daudio.h>

typedef struct {
    XAUDIO2_BUFFER buffer;
    WAVEFORMATEX format;
} SoundAsset_t;

class CAudioEngineXAudio : public IAudioEngine {
public:
	virtual bool Initialize(CClientAssetManager* pAssetManager) override;

    virtual void Update() override;

    virtual void SetMasterVolume(float fValue) override;

    virtual HVoice CreateVoice(SoundAsset_t sound) override;

	virtual HSound LoadSound(std::vector<uint8_t> vecSoundData) override;

	virtual HEmitter CreateEmitter(Vector3 vec3InitialPos) override;

	virtual void SetListenerPosition(Vector3 vec3NewPos) override;

	virtual void UpdateVoicePosition(HVoice hVoice, HEmitter hEmitter, Vector3 vec3Position) override;

private:
	X3DAUDIO_HANDLE m_h3dAudio;
	IXAudio2* pXAudio2;
	IXAudio2MasteringVoice* pMasterVoice;

	IXAudio2SubmixVoice* music_submix_voice_;

	XAUDIO2_SEND_DESCRIPTOR SFXSend{};
	XAUDIO2_VOICE_SENDS SFXSendList{};
    
	std::vector<IXAudio2SourceVoice*> m_vecPlayingSounds;

	X3DAUDIO_DISTANCE_CURVE_POINT volumePoints[3] = {
		{ 0.0f, 1.0f },
		{ 0.6f, 1.0f },
		{ 1.0f, 0.0f }
	};
	X3DAUDIO_DISTANCE_CURVE volumeCurve = {volumePoints, 3};
	X3DAUDIO_CONE m_listenerCone;
	X3DAUDIO_LISTENER m_listener;
	std::vector<X3DAUDIO_EMITTER> m_vecEmitters;
	
    std::vector<SoundAsset_t> m_vecSounds;
}