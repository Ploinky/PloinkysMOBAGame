#include "AudioSystem.h"

#include <string>
#include "Settings.h"
#include <common/PMG_Common.h>
#include "game/components/components.h"

AudioSystem::AudioSystem(IAudioEngine* pEngine, CClientAssetManager* pAssetManager) {
    m_pEngine = pEngine;
    m_pAssetManager = pAssetManager;
}

void AudioSystem::PlaySoundOnUnit(CClientGameState* pGameState, HSound hSound, UnitId idUnit) {
    HVoice hVoice = m_pEngine->CreateVoice(hSound);

    AudioEmitterComponent_t* pEmitterComp = pGameState->GetAudioEmitter(idUnit);

    if(pEmitterComp) {
        pEmitterComp->vecCurrentSounds.push_back(hVoice);
    }
}

void AudioSystem::Update(CClientGameState* pGameState, float fDelta) {
    for(auto& [id, audio] : pGameState->GetAllAudioEmitter()) {
        if(AudioEmitterComponent_t* pEmitterComp = pGameState->GetAudioEmitter(id)) {
            if(TransformComponent_t* pTransformComp = pGameState->GetTransform(id)) {
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

void AudioSystem::OnSpellHit(CClientGameState* pGameState, const CSpellHitEvent& hitEvent) {
    const CAbilityData& abilityData = m_pAssetManager->GetGameData().mapAbilityData.at(hitEvent.strSpellId);

    for(auto fx : abilityData.effect.vecFXEffects) {
        if(fx.strId.empty()) {
            continue;
        }

        const CEffectData& effectData = m_pAssetManager->GetGameData().mapEffectData.at(fx.strId);

        HSound hSound = m_pAssetManager->LoadSound(effectData.audioId, "");

        if(hSound == INVALID_HANDLE) {
            Logger::FormatErr("Failed to play sound <%s> on entity <%d>: sound was not loaded", effectData.audioId, hitEvent.idUnit);
            return;
        }

        PlaySoundOnUnit(pGameState, hSound, hitEvent.idUnit);
    }
}

void AudioSystem::OnAttackStart(CClientGameState* pGameState, const CAttackStartEvent& evt) {
    // TODO can only renderables play sound?
    RenderableComponent_t* pRenderable = pGameState->GetRenderable(evt.idUnit);
    if(pRenderable == nullptr) {
        Logger::FormatErr("Failed to play attack1 sound on entity <%d>: could not find entity type", evt.idUnit);
        return;
    }

    if(m_pAssetManager->GetGameData().mapCharacterData.find(pRenderable->strRenderable) == m_pAssetManager->GetGameData().mapCharacterData.end()) {
        Logger::FormatErr("Failed to play sound <%s> on entity <%d>: could not find character data for <%s>",
            "attack1", evt.idUnit, pRenderable->strRenderable);
        return;
    }

    CCharacterData charData = m_pAssetManager->GetGameData().mapCharacterData.at(pRenderable->strRenderable);

    if(!charData.optAudioData.has_value() || charData.optAudioData.value().mapAudioIds.find("attack1") == charData.optAudioData.value().mapAudioIds.end()) {
        Logger::FormatErr("Failed to play sound <%s> on entity <%d>: entity is missing that sound", "attack1", evt.idUnit);
        return;
    }

    std::string audioId = charData.optAudioData.value().mapAudioIds.at("attack1");
    HSound hAttackSound = m_pAssetManager->LoadSound(audioId, "");

    if(hAttackSound == INVALID_HANDLE) {
        Logger::FormatErr("Failed to play sound <%s> on entity <%d>: sound was not loaded", "attack1", evt.idUnit);
        return;
    }

    PlaySoundOnUnit(pGameState, hAttackSound, evt.idUnit);
}

void AudioSystem::OnEntityDeath(CClientGameState* pGameState, const CEntityDeathEvent& evt) {
    // TODO can only renderables play sound?
    RenderableComponent_t* pRenderable = pGameState->GetRenderable(evt.idUnit);
    if(pRenderable == nullptr) {
        Logger::FormatErr("Failed to play death sound on entity <%d>: could not find entity type", evt.idUnit);
        return;
    }

    if(m_pAssetManager->GetGameData().mapCharacterData.find(pRenderable->strRenderable) == m_pAssetManager->GetGameData().mapCharacterData.end()) {
        Logger::FormatErr("Failed to play sound <%s> on entity <%d>: could not find character data for <%s>",
            "death", evt.idUnit, pRenderable->strRenderable);
        return;
    }

    CCharacterData charData = m_pAssetManager->GetGameData().mapCharacterData.at(pRenderable->strRenderable);

    if(!charData.optAudioData.has_value() || charData.optAudioData.value().mapAudioIds.find("death") == charData.optAudioData.value().mapAudioIds.end()) {
        Logger::FormatErr("Failed to play sound <%s> on entity <%d>: entity is missing that sound", "death", evt.idUnit);
        return;
    }

    std::string audioId = charData.optAudioData.value().mapAudioIds.at("death");
    HSound hDeathSound = m_pAssetManager->LoadSound(audioId, "");

    if(hDeathSound == INVALID_HANDLE) {
        Logger::FormatErr("Failed to play sound <%s> on entity <%d>: sound was not loaded", "death", evt.idUnit);
        return;
    }

    PlaySoundOnUnit(pGameState, hDeathSound, evt.idUnit);
}