#include "spell-system.h"

#include "GameState.h"
#include "SpellTargetInfo.h"

CSpellSystem::CSpellSystem() {
    REGISTER_EVENT_HANDLER(CSpellAttemptCastEvent, OnSpellAttemptCast);
    REGISTER_EVENT_HANDLER(CSpellCastEvent, OnSpellCast);
    REGISTER_EVENT_HANDLER(CAttackIntentionEvent, OnAttackIntention);
}

void CSpellSystem::Update(CGameState* pGameState, float fDelta) {
    for(std::pair<UnitId, CGameObject*> pGameObj : pGameState->GameObjects) {
        CSpellCastComponent* pSpellComp = pGameObj.second->GetComponent<CSpellCastComponent>();

        if(pSpellComp == nullptr) {
            continue;
        }

        std::vector<SpellSlot_t>& vecSpells = pSpellComp->vecSpellSlots;

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

        if(!pSpellComp->optCurrentCast.has_value()) {
            continue;
        }

        ActiveCast_t& activeCast = pSpellComp->optCurrentCast.value();
        activeCast.fTimeInState += fDelta;

        CGameObject* pCaster = nullptr;
        CGameObject* pTarget = nullptr;
        CTransformComponent* pCasterTransform = nullptr;
        CTransformComponent* pTargetTransform = nullptr;
        CNavigationComponent* pNavigationComponent = nullptr;

        switch(activeCast.eState) {
            case ESpellCastState::IDLE:
            case ESpellCastState::APPROACHING:
                pCaster = pGameState->FindGameObjectById(activeCast.spellCtx->idCaster);
                pTarget = pGameState->FindGameObjectById(activeCast.spellCtx->idTarget);

                pCasterTransform = pCaster->GetComponent<CTransformComponent>();
                pTargetTransform = pTarget->GetComponent<CTransformComponent>();

                pNavigationComponent = pCaster->GetComponent<CNavigationComponent>();
                if(vecSpells[activeCast.nIndex].pSpell->fCastRange > (pCasterTransform->GetPosition() - pTargetTransform->GetPosition()).Length()) {
                    activeCast.eState = ESpellCastState::CASTING;
                    activeCast.fTimeInState = 0.0f;
                    
                    vecSpells[activeCast.nIndex].pSpell->OnCastStart(CSpellCastApi(pGameState), activeCast.spellCtx);

                    CSpellCastStartEvent* pStartEvt = new CSpellCastStartEvent(activeCast.spellCtx);
                    pGameState->VecEvent.emplace(pStartEvt);
                    break;
                }

                if(pNavigationComponent->m_vec3Destination != pTargetTransform->GetPosition()) {
                    pNavigationComponent->NavigateTo(pTargetTransform->GetPosition());
                }

                break;
            case ESpellCastState::CASTING:
                if(activeCast.fTimeInState >= vecSpells[activeCast.nIndex].pSpell->fCastPoint) {
                    pGameState->VecEvent.emplace(new CSpellCastEvent(activeCast.spellCtx));
                    activeCast.eState = ESpellCastState::BACKSWING;
                    activeCast.fTimeInState = 0.0f;
                }
                break;
            case ESpellCastState::CAST_POINT_REACHED:
            case ESpellCastState::BACKSWING:
                if (activeCast.fTimeInState >= vecSpells[activeCast.nIndex].pSpell->fCastTime - vecSpells[activeCast.nIndex].pSpell->fCastPoint) {
                    activeCast.eState = ESpellCastState::FINISHED;
                    pSpellComp->optCurrentCast.reset(); // Done
                }
            case ESpellCastState::FINISHED:
                vecSpells[activeCast.nIndex].fCooldownRemaining = vecSpells[activeCast.nIndex].pSpell->fCooldown;
                pGameState->VecEvent.emplace(new CCooldownStartedEvent(pGameObj.first, activeCast.nIndex, vecSpells[activeCast.nIndex].pSpell->fCooldown));
                pSpellComp->optCurrentCast.reset();
                break;
            case ESpellCastState::CANCELLED:
                pSpellComp->optCurrentCast.reset(); // Done
                break;
            default:
                break;
        }
    }
}

void CSpellSystem::OnSpellAttemptCast(CGameState* pGameState, CSpellAttemptCastEvent* pCastAttemptEvent) {
    Logger::FormatMsg("Received cast start event, processing...");

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

    CSpellCastComponent* pSpellComp = pCaster->GetComponent<CSpellCastComponent>();

    if(pSpellComp == nullptr) {
        Logger::FormatErr("Invalid cast start event: caster with id %d does not not have a spell cast component", pCastAttemptEvent->m_idCaster);
        return;
    }

    CSpellCastContext* pSpellCtx = new CSpellCastContext(pGameState);
    pSpellCtx->idCaster = pCastAttemptEvent->m_idCaster;
    pSpellCtx->idTarget = pCastAttemptEvent->m_targetInfo.target;
    pSpellCtx->nSpellIndex = pCastAttemptEvent->m_nIndex;
    TryCastSpell(pGameState, pSpellCtx);
}

void CSpellSystem::OnSpellCast(CGameState* pGameState, CSpellCastEvent* pCastEvent) {
    Logger::FormatMsg("Received cast event, processing...");

    CGameObject* pCaster = pGameState->FindGameObjectById(pCastEvent->m_spellCtx->idCaster);

    if(pCaster == nullptr) {
        Logger::FormatErr("Invalid cast event: caster with id %d does not exist", pCastEvent->m_spellCtx->idCaster);
        return;
    }

    CSpellCastComponent* pSpellComp = pCaster->GetComponent<CSpellCastComponent>();

    if(pSpellComp == nullptr) {
        Logger::FormatErr("Invalid cast event: caster with id %d does not not have a spell cast component", pCastEvent->m_spellCtx->idCaster);
        return;
    }

    pSpellComp->vecSpellSlots.at(pCastEvent->m_spellCtx->nSpellIndex).pSpell->OnCast(CSpellCastApi(pGameState), pCastEvent->m_spellCtx);

    if(pSpellComp->vecSpellSlots.at(pCastEvent->m_spellCtx->nSpellIndex).pSpell->eTargetType == ETargetingType::UNIT_INSTANT) {
        SpellHit(pGameState, pCastEvent->m_spellCtx);
    }
}

void CSpellSystem::SpellHit(CGameState* pGameState, CSpellCastContext* pCtx) {
    CGameObject* pCaster = pGameState->FindGameObjectById(pCtx->idCaster);

    CSpellCastComponent* pSpellComp = pCaster->GetComponent<CSpellCastComponent>();
    pSpellComp->vecSpellSlots.at(pCtx->nSpellIndex).pSpell->ApplyEffects(CSpellCastApi(pGameState), pCtx);

    pGameState->VecEvent.emplace(new CSpellHitEvent(pCtx->idTarget, pSpellComp->vecSpellSlots.at(pCtx->nSpellIndex).pSpell->idSpell));
}

void CSpellSystem::TryCastSpell(CGameState* pGameState, CSpellCastContext* pSpellCtx) {
    Logger::FormatMsg("casting spell no. %d", pSpellCtx->nSpellIndex);

    CGameObject* pCaster = pGameState->FindGameObjectById(pSpellCtx->idCaster);
    CGameObject* pTarget = pGameState->FindGameObjectById(pSpellCtx->idTarget);

    CSpellCastComponent* pCastComponent = pCaster->GetComponent<CSpellCastComponent>();

    if(pSpellCtx->nSpellIndex >= pCastComponent->vecSpellSlots.size()) {
        // TODO ?
        Logger::FormatErr("failed to cast spell %d, incorrect spell index", pSpellCtx->nSpellIndex);
        return;
    }

    SpellSlot_t& spellSlot = pCastComponent->vecSpellSlots.at(pSpellCtx->nSpellIndex);
    
    if(spellSlot.pSpell == nullptr) {
        Logger::FormatErr("failed to cast spell %d, invalid spell pointer", pSpellCtx->nSpellIndex);
        return;
    }

    if(pCastComponent->optCurrentCast.has_value()) {
        Logger::FormatErr("failed to cast spell %d, already casting something", pSpellCtx->nSpellIndex);
        return;
    }

    if(spellSlot.fCooldownRemaining > 0) {
        Logger::FormatErr("failed to cast spell %d, cooldown", pSpellCtx->nSpellIndex);
        return;
    }

    // TODO stop movement
    pCaster->GetComponent<CNavigationComponent>()->StopNavigation();
    pCaster->GetComponent<CMovementComponent>()->ClearTarget();
    Vector3 pos = pCaster->GetComponent<CTransformComponent>()->GetPosition();
    pGameState->VecEvent.emplace(new CMoveIntentionEvent(pCaster->GetId(), pos, 0));

    ActiveCast_t cast;
    cast.eState = ESpellCastState::IDLE;
    cast.fTimeInState = 0.0f;
    cast.nIndex = pSpellCtx->nSpellIndex;
    cast.spellCtx = pSpellCtx;
    pCastComponent->optCurrentCast.emplace(cast);
}


void CSpellSystem::OnAttackIntention(CGameState* pGameState, CAttackIntentionEvent* pEvt) {
    CGameObject* pAttacker = pGameState->FindGameObjectById(pEvt->idUnit);

    if(pAttacker == nullptr) {
        Logger::FormatErr("Invalid attack intention event: unit with id %d does not exist", pEvt->idTarget);
        return;
    }

    CSpellCastComponent* pSpellComp = pAttacker->GetComponent<CSpellCastComponent>();

    if(pSpellComp == nullptr) {
        return;
    }

    pSpellComp->optCurrentCast.reset();
}


void CSpellSystem::OnDeath(CGameState* pGameState, CDeathEvent* pDeathEvent) {
    CGameObject* pDead = pGameState->FindGameObjectById(pDeathEvent->idTarget);

    if(pDead == nullptr) {
        Logger::FormatErr("Invalid death event: unit with id %d does not exist", pDeathEvent->idTarget);
        return;
    }

    CSpellCastComponent* pSpellComp = pDead->GetComponent<CSpellCastComponent>();

    if(pSpellComp == nullptr) {
        return;
    }

    pSpellComp->optCurrentCast.reset();
}