#pragma once

#include <xaudio2.h>

namespace PMG {
	class AudioSystem {
	public:
		bool Initialize();
		bool StartPlayingSound();
		void Update();

	private:
		IXAudio2* pXAudio2;
		IXAudio2MasteringVoice* pMasterVoice;
		IXAudio2SourceVoice* pSourceVoice;

		IXAudio2SubmixVoice* music_submix_voice_;
		XAUDIO2_SEND_DESCRIPTOR SFXSend{};
		XAUDIO2_VOICE_SENDS SFXSendList{};
	};
}