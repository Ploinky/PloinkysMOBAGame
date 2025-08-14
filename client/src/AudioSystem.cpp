#include "AudioSystem.h"

#include <combaseapi.h>
#include <string>
#include "Settings.h"
#include <Common/PMG_Common.h>
#include "components/components.h"

AudioSystem::AudioSystem(CAudioEngine* pEngine, CClientAssetManager* pAssetManager) {
    m_pEngine = pEngine;
    m_pAssetManager = pAssetManager;

    REGISTER_EVENT_HANDLER(CSpellHitEvent, OnSpellHit)
    REGISTER_EVENT_HANDLER(CAttackStartEvent, OnAttackStart)
}

void AudioSystem::PlaySoundOnUnit(HSound hSound, UnitId idUnit) {
    SoundAsset_t& sound = m_pAssetManager->GetSound(hSound);
    HVoice hVoice = m_pEngine->CreateVoice(sound);
    
}

void AudioSystem::Update(CGameState* pGameState, float fDelta) {
    for(UnitId idUnit : pGameState->vecUnits) {
        if(AudioEmitterComponent_t* pEmitterComp = pGameState->GetComponent<AudioEmitterComponent_t>(idUnit)) {
            if(TransformComponent_t* pTransformComp = pGameState->GetComponent<TransformComponent_t>(idUnit)) {
                for(HVoice hVoice : pEmitterComp->vecCurrentSounds) {
                    m_pEngine->UpdateVoicePosition(hVoice, pEmitterComp->hEmitter, pTransformComp->vec3Position);
                }
            }
        }
    }
}

void AudioSystem::SetListenerPosition(Vector3 vec3LisPos) {
    m_pEngine->SetListenerPosition(vec3LisPos);
}

void AudioSystem::OnSpellHit(CGameState* pGameState, CSpellHitEvent* pHitEvent) {
    SoundAsset_t& sound = m_pAssetManager->GetSound(pHitEvent->hSound);
    HVoice hVoice = m_pEngine->CreateVoice(sound);

    AudioEmitterComponent_t* pEmitterComp = pGameState->GetComponent<AudioEmitterComponent_t>(pHitEvent->idUnit);
    pEmitterComp->vecCurrentSounds.push_back(hVoice);
}

void AudioSystem::OnAttackStart(CGameState* pGameState, CAttackStartEvent* pHitEvent) {
    SoundAsset_t& sound = m_pAssetManager->GetSound(pHitEvent->hSound);
    HVoice hVoice = m_pEngine->CreateVoice(sound);

    AudioEmitterComponent_t* pEmitterComp = pGameState->GetComponent<AudioEmitterComponent_t>(pHitEvent->idUnit);
    pEmitterComp->vecCurrentSounds.push_back(hVoice);
}