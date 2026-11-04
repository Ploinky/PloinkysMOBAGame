#include "spell-system.h"

#include "../game/server-game-state.h"
#include "../SpellTargetInfo.h"
#include "../spell-cast-api.h"

CSpellSystem::CSpellSystem() {
}

void CSpellSystem::Update(CServerGameState* pGameState, float fDelta) {
    for(auto& [id, spellComp] : pGameState->GetAllSpellCast()) {

        std::vector<SpellSlot_t>& vecSpells = spellComp.vecSpellSlots;

        for(int i = 0; i < vecSpells.size(); i++) {
            SpellSlot_t& spellSlot = vecSpells.at(i);
            if(spellSlot.fCooldownRemaining > 0.0f) {
                if(spellSlot.fCooldownRemaining - fDelta > 0.0f) {
                    spellSlot.fCooldownRemaining = spellSlot.fCooldownRemaining - fDelta;
                } else {
                    spellSlot.fCooldownRemaining = 0.0f;
                }
            }
        }

        if(!spellComp.optCurrentCast.has_value()) {
            continue;
        }
        

        ActiveCast_t& activeCast = spellComp.optCurrentCast.value();
        activeCast.fTimeInState += fDelta;

        if(pGameState->GetHealth(activeCast.spellCtx->idCaster) && pGameState->GetHealth(activeCast.spellCtx->idCaster)->bIsDead) {
            spellComp.optCurrentCast.reset();
            continue;
        }

        TransformComponent_t* pCasterTransform = nullptr;
        TransformComponent_t* pTargetTransform = nullptr;
        NavigationComponent_t* pNavigationComponent = nullptr;

        switch(activeCast.eState) {
            case ESpellCastState::IDLE:
            case ESpellCastState::APPROACHING:
                pCasterTransform = pGameState->GetTransform(activeCast.spellCtx->idCaster);
                if (vecSpells[activeCast.nIndex].data.eTargetType == EAbilityTargetType::POINT) {
                    activeCast.eState = ESpellCastState::CASTING;
                    activeCast.fTimeInState = 0.0f;
                    pGameState->EmitEvent(CSpellCastStartEvent(activeCast.spellCtx));
                    pGameState->EmitEvent(CMoveEvent(
                        activeCast.spellCtx->idCaster,
                        pCasterTransform->GetPosition(),
                        CalculateAngle(Vector2(pCasterTransform->GetPosition().x, pCasterTransform->GetPosition().z), activeCast.spellCtx->vec2Target)
                    ));

                    break;
                }
                pTargetTransform = pGameState->GetTransform(activeCast.spellCtx->idTarget);

                pNavigationComponent = pGameState->GetNavigation(activeCast.spellCtx->idCaster);
                if(vecSpells[activeCast.nIndex].data.fCastRange > (pCasterTransform->GetPosition() - pTargetTransform->GetPosition()).Length()) {
                    activeCast.eState = ESpellCastState::CASTING;
                    activeCast.fTimeInState = 0.0f;
                    
                    // TODO
                    // vecSpells[activeCast.nIndex].pSpell->OnCastStart(CSpellCastApi(pGameState), activeCast.spellCtx);

                    pGameState->EmitEvent(CSpellCastStartEvent(activeCast.spellCtx));
                    break;
                }

                if(pNavigationComponent->vec3Destination != pTargetTransform->GetPosition()) {
                    pNavigationComponent->vec3Destination = pTargetTransform->GetPosition();
                    pNavigationComponent->eStatus = ENavigationStatus::PATHING;
                }

                break;
            case ESpellCastState::CASTING:
                if(activeCast.fTimeInState >= vecSpells[activeCast.nIndex].data.fCastPoint) {
                    pGameState->EmitEvent(CSpellCastEvent(activeCast.spellCtx));
                    activeCast.eState = ESpellCastState::BACKSWING;
                    activeCast.fTimeInState = 0.0f;
                    vecSpells[activeCast.nIndex].fCooldownRemaining = vecSpells[activeCast.nIndex].data.fCooldown;
                    pGameState->EmitEvent(CCooldownStartedEvent(id, activeCast.nIndex, vecSpells[activeCast.nIndex].data.fCooldown));
                }
                break;
            case ESpellCastState::CAST_POINT_REACHED:
            case ESpellCastState::BACKSWING:
                if (activeCast.fTimeInState >= vecSpells[activeCast.nIndex].data.fCastTime - vecSpells[activeCast.nIndex].data.fCastPoint) {
                    activeCast.eState = ESpellCastState::FINISHED;
                    spellComp.optCurrentCast.reset(); // Done
                }
            case ESpellCastState::FINISHED:
                spellComp.optCurrentCast.reset();
                break;
            case ESpellCastState::CANCELLED:
                spellComp.optCurrentCast.reset(); // Done
                break;
            default:
                break;
        }
    }
}

void CSpellSystem::OnSpellAttemptCast(CServerGameState* pGameState, const CSpellAttemptCastEvent& evt) {
    SpellCastComponent_t* pSpellComp = pGameState->GetSpellCast(evt.m_idCaster);

    if(pSpellComp == nullptr) {
        Logger::FormatErr("Invalid cast start event: caster with id %d does not not have a spell cast component", evt.m_idCaster);
        return;
    }

    CSpellCastContext* pSpellCtx = new CSpellCastContext();
    pSpellCtx->idCaster = evt.m_idCaster;
    pSpellCtx->idTarget = evt.m_targetInfo.target;
    pSpellCtx->vec2Target = Vector2(evt.m_targetInfo.target_point.x, evt.m_targetInfo.target_point.z);
    pSpellCtx->nSpellIndex = evt.m_nIndex;
    TryCastSpell(pGameState, pSpellCtx);
}

void CSpellSystem::OnSpellCast(CServerGameState* pGameState, const CSpellCastEvent& evt) {
    SpellCastComponent_t* pSpellComp = pGameState->GetSpellCast(evt.m_spellCtx->idCaster);

    if(pSpellComp == nullptr) {
        Logger::FormatErr("Invalid cast event: caster with id %d does not not have a spell cast component", evt.m_spellCtx->idCaster);
        return;
    }

    // TODO
    // pSpellComp->vecSpellSlots.at(evt.m_spellCtx->nSpellIndex).pSpell->OnCast(CSpellCastApi(pGameState), evt.m_spellCtx);

    if(pSpellComp->vecSpellSlots.at(evt.m_spellCtx->nSpellIndex).data.eTargetType == EAbilityTargetType::UNIT) {
        SpellHit(pGameState, evt.m_spellCtx);
    } else if (pSpellComp->vecSpellSlots.at(evt.m_spellCtx->nSpellIndex).data.eTargetType == EAbilityTargetType::POINT) {
        SpellHit(pGameState, evt.m_spellCtx);
    }
}

void CSpellSystem::SpellHit(CServerGameState* pGameState, CSpellCastContext* pCtx) {
    SpellCastComponent_t* pSpellComp = pGameState->GetSpellCast(pCtx->idCaster);
    SpellSlot_t spell = pSpellComp->vecSpellSlots.at(pCtx->nSpellIndex);

    CSpellCastApi api = CSpellCastApi(pGameState);
    for(ImpactEffectDamage_t damageEffect : spell.data.effect.vecDamageEffects) {
        api.ApplyDamage(pCtx->idCaster, pCtx->idTarget, damageEffect.vecDamage[0]);
    }

    for(ImpactEffectHeal_t healEffect : spell.data.effect.vecHealEffects) {
        api.ApplyHeal(pCtx->idCaster, pCtx->idTarget, healEffect.vecHeal[0]);
    }

    for (ImpactEffectSpawn_t spawnEffect : spell.data.effect.vecSpawnEffects) {
        api.SpawnEntity(pCtx->idCaster, spawnEffect.strTemplateId, pCtx->vec2Target);
    }

    pGameState->EmitEvent(CSpellHitEvent(pCtx->idTarget, pSpellComp->vecSpellSlots.at(pCtx->nSpellIndex).data.strId));
}

void CSpellSystem::SpellPointHit(CServerGameState* pGameState, SpellCastContext_t context) {
    CSpellCastApi api = CSpellCastApi(pGameState);
    for(ImpactEffectSpawn_t spawn : context.abilityData.effect.vecSpawnEffects) {
        api.SpawnEntity(context.idCaster, spawn.strTemplateId, context.vec2Target);
    }
}

void CSpellSystem::TryCastSpell(CServerGameState* pGameState, CSpellCastContext* pSpellCtx) {
    SpellCastComponent_t* pCastComponent = pGameState->GetSpellCast(pSpellCtx->idCaster);

    if(pSpellCtx->nSpellIndex >= pCastComponent->vecSpellSlots.size()) {
        // TODO ?
        Logger::FormatErr("failed to cast spell %d, incorrect spell index", pSpellCtx->nSpellIndex);
        return;
    }

    SpellSlot_t& spellSlot = pCastComponent->vecSpellSlots.at(pSpellCtx->nSpellIndex);
    
    if(spellSlot.data.strId == "") {
        Logger::FormatErr("failed to cast spell %d, invalid spell pointer", pSpellCtx->nSpellIndex);
        return;
    }

    if(pCastComponent->optCurrentCast.has_value()) {
        return;
    }

    if(spellSlot.fCooldownRemaining > 0) {
        return;
    }

    // TODO stop movement

    pGameState->GetNavigation(pSpellCtx->idCaster)->eStatus = ENavigationStatus::IDLE;
    pGameState->GetNavigation(pSpellCtx->idCaster)->vec3Destination = Vector3::ZERO;
    if(MovementComponent_t* pMovement = pGameState->GetMovement(pSpellCtx->idCaster)) {
        if(TransformComponent_t* pTransform = pGameState->GetTransform(pSpellCtx->idCaster)) {
            pMovement->vec3Target = pTransform->GetPosition();
        }
    }
    Vector3 pos = pGameState->GetTransform(pSpellCtx->idCaster)->GetPosition();
    pGameState->EmitEvent(CMoveIntentionEvent(pSpellCtx->idCaster, pos, 0));

    ActiveCast_t cast;
    cast.eState = ESpellCastState::IDLE;
    cast.fTimeInState = 0.0f;
    cast.nIndex = pSpellCtx->nSpellIndex;
    cast.spellCtx = pSpellCtx;
    pCastComponent->optCurrentCast.emplace(cast);
}


void CSpellSystem::OnAttackIntention(CServerGameState* pGameState, const CAttackIntentionEvent& evt) {
    SpellCastComponent_t* pSpellComp = pGameState->GetSpellCast(evt.idUnit);

    if(pSpellComp == nullptr) {
        return;
    }

    pSpellComp->optCurrentCast.reset();
}


void CSpellSystem::OnDeath(CServerGameState* pGameState, const CDeathEvent& evt) {
    SpellCastComponent_t* pSpellComp = pGameState->GetSpellCast(evt.idTarget);

    if(pSpellComp == nullptr) {
        return;
    }

    pSpellComp->optCurrentCast.reset();
}

void CSpellSystem::OnUseEntity(CServerGameState* pGameState, const CUseEntityEvent& evt) {
    UseableComponent_t* pUseable = pGameState->GetUseable(evt.idEntity);

    SpellCastContext_t context {
        .idCaster = evt.idUser,
        .vec2Target = {evt.x, evt.y},
        .abilityData = pUseable->abilityData
    };

    SpellPointHit(pGameState, context);
}