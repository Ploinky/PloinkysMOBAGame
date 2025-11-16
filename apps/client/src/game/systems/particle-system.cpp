#include "particle-system.h"

#include "game/client-game-state.h"
#include "game/components/components.h"
#include "Model.h"
#include "ParticleEmitter.h"
#include "ParticleEffect.h"

CParticleSystem::CParticleSystem(CClientAssetManager* pAssetManager) {
    m_pAssetManager = pAssetManager;
}

void CParticleSystem::Update(CClientGameState* pGameState, float fDelta) {
    for(const UnitId& id : pGameState->vecUnits) {
        ParticleComponent_t* pParticleComp = pGameState->GetParticle(id);

        if(pParticleComp == nullptr) {
            return;
        }

        auto it = pParticleComp->vecEffects.begin();
        while(it != pParticleComp->vecEffects.end()) {
            ParticleEffect* pEffect = *it;

            if(pEffect->attached_to_ != nullptr) {
                TransformComponent_t* pAttachedToTransform = pGameState->GetTransform(pEffect->attached_to_->unit_id);

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
                it = pParticleComp->vecEffects.erase(it);
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