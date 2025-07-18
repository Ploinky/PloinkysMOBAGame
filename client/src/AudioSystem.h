#pragma once

#include <xaudio2.h>
#include <x3daudio.h>
#include <map>
#include <string>
#include <Common/AssetManager.h>

class AudioComponent {
public:
	std::string fileName;
	bool isPlaying;
	IXAudio2SourceVoice* pSourceVoice;
	bool shouldStopPlaying;
};

struct Sound {
	XAUDIO2_BUFFER Buffer;
	WAVEFORMATEX Format;
};

class AudioSystem {
public:
	void Update();
	bool Initialize(AssetManager* pAssetManager);
	void StartPlayingSound(std::string strSoundName);

	void SetMasterVolume(float value);

private:
	X3DAUDIO_HANDLE m_h3dAudio;
	IXAudio2* pXAudio2;
	IXAudio2MasteringVoice* pMasterVoice;

	IXAudio2SubmixVoice* music_submix_voice_;
	XAUDIO2_SEND_DESCRIPTOR SFXSend{};
	XAUDIO2_VOICE_SENDS SFXSendList{};

	std::map<std::string, Sound*> m_mapSounds;
	std::vector<IXAudio2SourceVoice*> m_vecPlayingSounds;

	void LoadSoundFile(std::string strSoundName, AssetManager* pAssetManager);
};
