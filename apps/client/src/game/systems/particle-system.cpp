#include "particle-system.h"

#include "../client-game-state.h"
#include "../components/components.h"
#include "../../Model.h"
#include "../../ParticleEmitter.h"
#include "../../ParticleEffect.h"

CParticleSystem::CParticleSystem(CClientAssetManager* pAssetManager) {
    m_pAssetManager = pAssetManager;
}

void CParticleSystem::Update(CClientGameState* pGameState, float fDelta) {
    for(auto& [id, particleComp] : pGameState->GetAllParticle()) {
        auto it = particleComp.vecEffects.begin();
        while(it != particleComp.vecEffects.end()) {
            ParticleEffect* pEffect = *it;

            if(pEffect->attached_to_ != UNIT_ID_NONE) {
                TransformComponent_t* pAttachedToTransform = pGameState->GetTransform(pEffect->attached_to_);

                pEffect->position = pAttachedToTransform->vec3Position;
                pEffect->position.y += 100.0f;
            }
            
            pEffect->Update(fDelta);


            bool bIsNotDone = false;

            for(ParticleEmitter* pEmitter : pEffect->emitters_) {
                if(!pEmitter->IsDone()) {
                    bIsNotDone = true;
                }
            }

            if(!bIsNotDone) {
                pEffect->destroy = true;
                it = particleComp.vecEffects.erase(it);
            } else {
                it++;
            }

        }
        /*
        for(int i = 0; i < pParticleComp->vecEffects.size(); i++) {
            ParticleEffect* pEffect = pParticleComp->vecEffects[i];
            if(pEffect->attached_to_ != nullptr) {
                TransformComponent_t* pAttachedToTransform = pGameState->GetTransform(pEffect->attached_to_->unit_id);

                pEffect->position = pAttachedToTransform->vec3Position;
            }

            bool bIsNotDone = false;

            for(ParticleEmitter* pEmitter : pEffect->emitters_) {
                if(!pEmitter->IsDone()) {
                    bIsNotDone = true;
                }
            }

            if(!bIsNotDone) {
                pEffect->destroy = true;
                pParticleComp->vecEffects.erase(pEffect);
            }
        }
        */

    }
}


void CParticleSystem::OnSpellHit(CClientGameState* pGameState, const CSpellHitEvent& evt) {
    ParticleEffect* particle_system = ParticleEffect::Load("assets/characters/stormcaller/abilities/" + evt.strSpellId + ".pts", m_pAssetManager);

    if(!pGameState->GetParticle(evt.idUnit)) {
        pGameState->AddParticle(evt.idUnit);
    }

    if(evt.idUnit != UNIT_ID_NONE) {
        particle_system->Attach(evt.idUnit);
    }

    ParticleComponent_t* pParticleComp = pGameState->GetParticle(evt.idUnit);
    pParticleComp->vecEffects.push_back(particle_system);
}