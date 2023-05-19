#pragma once

#include <xaudio2.h>
#include "systems.h"
#include <string>

namespace PMG {
	class AudioComponent {
	public:
		std::wstring fileName;
		bool isPlaying;
		IXAudio2SourceVoice* pSourceVoice;
	};

	class AudioSystem : public PMGSystem {
	public:
		void Update();
		bool Initialize();
		bool StartPlayingSound();

	private:
		IXAudio2* pXAudio2;
		IXAudio2MasteringVoice* pMasterVoice;
		IXAudio2SourceVoice* pSourceVoice;

		IXAudio2SubmixVoice* music_submix_voice_;
		XAUDIO2_SEND_DESCRIPTOR SFXSend{};
		XAUDIO2_VOICE_SENDS SFXSendList{};
	};
}