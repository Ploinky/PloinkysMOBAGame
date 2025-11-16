#include "spell-system.h"

#include "game/server-game-state.h"
#include "SpellTargetInfo.h"
#include "spell-cast-api.h"

CSpellSystem::CSpellSystem() {
    REGISTER_EVENT_HANDLER(CSpellAttemptCastEvent, OnSpellAttemptCast);
    REGISTER_EVENT_HANDLER(CSpellCastEvent, OnSpellCast);
    REGISTER_EVENT_HANDLER(CAttackIntentionEvent, OnAttackIntention);
}

void CSpellSystem::Update(CServerGameState* pGameState, float fDelta) {
    for(CSpellCastComponent& spellComp : pGameState->GetAllSpellCast()) {

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

        CGameObject* pCaster = pGameState->FindGameObjectById(activeCast.spellCtx->idCaster);
        if(pGameState->GetHealth(pCaster->GetId()) && pGameState->GetHealth(pCaster->GetId())->bIsDead) {
            spellComp.optCurrentCast.reset();
            continue;
        }

        CGameObject* pTarget = nullptr;
        CTransformComponent* pCasterTransform = nullptr;
        CTransformComponent* pTargetTransform = nullptr;
        CNavigationComponent* pNavigationComponent = nullptr;

        switch(activeCast.eState) {
            case ESpellCastState::IDLE:
            case ESpellCastState::APPROACHING:
                pTarget = pGameState->FindGameObjectById(activeCast.spellCtx->idTarget);

                pCasterTransform = pGameState->GetTransform(pCaster->GetId());
                pTargetTransform = pGameState->GetTransform(pTarget->GetId());

                pNavigationComponent = pGameState->GetNavigation(pCaster->GetId());
                if(vecSpells[activeCast.nIndex].data.fCastRange > (pCasterTransform->GetPosition() - pTargetTransform->GetPosition()).Length()) {
                    activeCast.eState = ESpellCastState::CASTING;
                    activeCast.fTimeInState = 0.0f;
                    
                    // TODO
                    // vecSpells[activeCast.nIndex].pSpell->OnCastStart(CSpellCastApi(pGameState), activeCast.spellCtx);

                    CSpellCastStartEvent* pStartEvt = new CSpellCastStartEvent(activeCast.spellCtx);
                    pGameState->VecEvent.emplace(pStartEvt);
                    break;
                }

                if(pNavigationComponent->vec3Destination != pTargetTransform->GetPosition()) {
                    pNavigationComponent->vec3Destination = pTargetTransform->GetPosition();
                    pNavigationComponent->bIsNavigating = true;
                }

                break;
            case ESpellCastState::CASTING:
                if(activeCast.fTimeInState >= vecSpells[activeCast.nIndex].data.fCastPoint) {
                    pGameState->VecEvent.emplace(new CSpellCastEvent(activeCast.spellCtx));
                    activeCast.eState = ESpellCastState::BACKSWING;
                    activeCast.fTimeInState = 0.0f;
                }
                break;
            case ESpellCastState::CAST_POINT_REACHED:
            case ESpellCastState::BACKSWING:
                if (activeCast.fTimeInState >= vecSpells[activeCast.nIndex].data.fCastTime - vecSpells[activeCast.nIndex].data.fCastPoint) {
                    activeCast.eState = ESpellCastState::FINISHED;
                    spellComp.optCurrentCast.reset(); // Done
                }
            case ESpellCastState::FINISHED:
                vecSpells[activeCast.nIndex].fCooldownRemaining = vecSpells[activeCast.nIndex].data.fCooldown;
                pGameState->VecEvent.emplace(new CCooldownStartedEvent(spellComp.idUnit, activeCast.nIndex, vecSpells[activeCast.nIndex].data.fCooldown));
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

void CSpellSystem::OnSpellAttemptCast(CServerGameState* pGameState, CSpellAttemptCastEvent* pCastAttemptEvent) {
    CGameObject* pCaster = pGameState->FindGameObjectById(pCastAttemptEvent->m_idCaster);

    if(pCaster == nullptr) {
        Logger::FormatErr("Invalid cast start event: caster with id %d does not exist", pCastAttemptEvent->m_idCaster);
        return;
    }

    CGameObject* pTarget = pGameState->FindGameObjectById(pCastAttemptEvent->m_targetInfo.target);

    if(pTarget == nullptr) {
        Logger::FormatErr("Invalid cast start event: target with id %d does not exist", pCastAttemptEvent->m_targetInfo.target);
        return;
    }

    CSpellCastComponent* pSpellComp = pGameState->GetSpellCast(pCaster->GetId());

    if(pSpellComp == nullptr) {
        Logger::FormatErr("Invalid cast start event: caster with id %d does not not have a spell cast component", pCastAttemptEvent->m_idCaster);
        return;
    }

    CSpellCastContext* pSpellCtx = new CSpellCastContext();
    pSpellCtx->idCaster = pCastAttemptEvent->m_idCaster;
    pSpellCtx->idTarget = pCastAttemptEvent->m_targetInfo.target;
    pSpellCtx->nSpellIndex = pCastAttemptEvent->m_nIndex;
    TryCastSpell(pGameState, pSpellCtx);
}

void CSpellSystem::OnSpellCast(CServerGameState* pGameState, CSpellCastEvent* pCastEvent) {
    CGameObject* pCaster = pGameState->FindGameObjectById(pCastEvent->m_spellCtx->idCaster);

    if(pCaster == nullptr) {
        Logger::FormatErr("Invalid cast event: caster with id %d does not exist", pCastEvent->m_spellCtx->idCaster);
        return;
    }

    CSpellCastComponent* pSpellComp = pGameState->GetSpellCast(pCaster->GetId());

    if(pSpellComp == nullptr) {
        Logger::FormatErr("Invalid cast event: caster with id %d does not not have a spell cast component", pCastEvent->m_spellCtx->idCaster);
        return;
    }

    // TODO
    // pSpellComp->vecSpellSlots.at(pCastEvent->m_spellCtx->nSpellIndex).pSpell->OnCast(CSpellCastApi(pGameState), pCastEvent->m_spellCtx);

    if(pSpellComp->vecSpellSlots.at(pCastEvent->m_spellCtx->nSpellIndex).data.eTargetType == EAbilityTargetType::UNIT) {
        SpellHit(pGameState, pCastEvent->m_spellCtx);
    }
}

void CSpellSystem::SpellHit(CServerGameState* pGameState, CSpellCastContext* pCtx) {
    CGameObject* pCaster = pGameState->FindGameObjectById(pCtx->idCaster);

    CSpellCastComponent* pSpellComp = pGameState->GetSpellCast(pCaster->GetId());
    SpellSlot_t spell = pSpellComp->vecSpellSlots.at(pCtx->nSpellIndex);

    for(ImpactEffectDamage_t damageEffect : spell.data.effect.vecDamageEffects) {
        CSpellCastApi api = CSpellCastApi(pGameState);
        api.ApplyDamage(pCtx->idCaster, pCtx->idTarget, damageEffect.vecDamage[0]);
    }

    pGameState->VecEvent.emplace(new CSpellHitEvent(pCtx->idTarget, pSpellComp->vecSpellSlots.at(pCtx->nSpellIndex).data.strId));
}

void CSpellSystem::TryCastSpell(CServerGameState* pGameState, CSpellCastContext* pSpellCtx) {
    CGameObject* pCaster = pGameState->FindGameObjectById(pSpellCtx->idCaster);
    CGameObject* pTarget = pGameState->FindGameObjectById(pSpellCtx->idTarget);

    CSpellCastComponent* pCastComponent = pGameState->GetSpellCast(pCaster->GetId());

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

    pGameState->GetNavigation(pCaster->GetId())->bIsNavigating = false;
    pGameState->GetNavigation(pCaster->GetId())->vec3Destination = Vector3::ZERO;
    if(CMovementComponent* pMovement = pGameState->GetMovement(pGameState->GetNavigation(pCaster->GetId())->idUnit)) {
        if(CTransformComponent* pTransform = pGameState->GetTransform(pGameState->GetNavigation(pCaster->GetId())->idUnit)) {
            pMovement->vec3Target = pTransform->GetPosition();
        }
    }
    Vector3 pos = pGameState->GetTransform(pCaster->GetId())->GetPosition();
    pGameState->VecEvent.emplace(new CMoveIntentionEvent(pCaster->GetId(), pos, 0));

    ActiveCast_t cast;
    cast.eState = ESpellCastState::IDLE;
    cast.fTimeInState = 0.0f;
    cast.nIndex = pSpellCtx->nSpellIndex;
    cast.spellCtx = pSpellCtx;
    pCastComponent->optCurrentCast.emplace(cast);
}


void CSpellSystem::OnAttackIntention(CServerGameState* pGameState, CAttackIntentionEvent* pEvt) {
    CGameObject* pAttacker = pGameState->FindGameObjectById(pEvt->idUnit);

    if(pAttacker == nullptr) {
        Logger::FormatErr("Invalid attack intention event: unit with id %d does not exist", pEvt->idTarget);
        return;
    }

    CSpellCastComponent* pSpellComp = pGameState->GetSpellCast(pAttacker->GetId());

    if(pSpellComp == nullptr) {
        return;
    }

    pSpellComp->optCurrentCast.reset();
}


void CSpellSystem::OnDeath(CServerGameState* pGameState, CDeathEvent* pDeathEvent) {
    CGameObject* pDead = pGameState->FindGameObjectById(pDeathEvent->idTarget);

    if(pDead == nullptr) {
        Logger::FormatErr("Invalid death event: unit with id %d does not exist", pDeathEvent->idTarget);
        return;
    }

    CSpellCastComponent* pSpellComp = pGameState->GetSpellCast(pDead->GetId());

    if(pSpellComp == nullptr) {
        return;
    }

    pSpellComp->optCurrentCast.reset();
}