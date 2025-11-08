#include <core/audio/audio-engine-minaudio.h>

IAudioEngine* IAudioEngine::Create() {
    return new CAudioEngineMinaudio();
}


bool CAudioEngineMinaudio::Initialize() {
    return true;
}

void CAudioEngineMinaudio::Update() {

}

void CAudioEngineMinaudio::SetMasterVolume(float fValue) {

}

HSound CAudioEngineMinaudio::LoadSound(std::vector<uint8_t> vecSoundData) {
    return INVALID_ASSET_HANDLE;
}
HVoice CAudioEngineMinaudio::CreateVoice(HSound sound) {
    return INVALID_VOICE_HANDLE;
}

HEmitter CAudioEngineMinaudio::CreateEmitter(Vector3 vec3InitialPos) {
    return INVALID_EMITTER_HANDLE;
}

void CAudioEngineMinaudio::SetListenerPosition(Vector3 vec3NewPos) {

}

void CAudioEngineMinaudio::UpdateVoicePosition(HVoice hVoice, HEmitter hEmitter, Vector3 vec3Position) {

}