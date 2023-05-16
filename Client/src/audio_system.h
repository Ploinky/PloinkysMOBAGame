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
	};
}