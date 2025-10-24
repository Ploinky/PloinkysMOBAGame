#include "systems/attack-system.h"

CAttackSystem::CAttackSystem() {
    REGISTER_EVENT_HANDLER(CAttackIntentionEvent, OnAttackIntention)
    REGISTER_EVENT_HANDLER(CAttackHitEvent, OnAttackHit)
    REGISTER_EVENT_HANDLER(CMoveIntentionEvent, OnMoveIntention)
    REGISTER_EVENT_HANDLER(CSpellAttemptCastEvent, OnSpellCastAttempt)
}

void CAttackSystem::Update(CGameState* pGameState, float fDelta) {
    for(std::pair<UnitId, CGameObject*> pGameObj : pGameState->GameObjects) {
        CBasicAttackComponent* pAttackComp = pGameObj.second->GetComponent<CBasicAttackComponent>();

        if(pAttackComp == nullptr) {
            continue;
        }

        if(!pAttackComp->optCurrentAttack.has_value()) {
            continue;
        }

        ActiveAttack_t& activeAttack = pAttackComp->optCurrentAttack.value();
        activeAttack.fTimeInState += fDelta;

        CGameObject* pAttacker = pGameObj.second;
        CGameObject* pTarget = pGameState->FindGameObjectById(activeAttack.idTarget);
        CTransformComponent* pAttackerTransform = nullptr;
        CTransformComponent* pTargetTransform = nullptr;
        CNavigationComponent* pNavigationComponent = nullptr;

        if(pTarget == nullptr) {
            pAttackComp->optCurrentAttack.reset();
            continue;
        }

        if(CHealthComponent* pHealthComp = pTarget->GetComponent<CHealthComponent>()) {
            if(pHealthComp->bIsDead) {
                pAttackComp->optCurrentAttack.reset();
                continue;
            }
        }

        switch(activeAttack.eState) {
            case EAttackState::IDLE:
            case EAttackState::APPROACHING:
                pAttackerTransform = pAttacker->GetComponent<CTransformComponent>();
                pTargetTransform = pTarget->GetComponent<CTransformComponent>();

                pNavigationComponent = pAttacker->GetComponent<CNavigationComponent>();
                if(pAttackComp->fRange > (pAttackerTransform->GetPosition() - pTargetTransform->GetPosition()).Length()) {
                    activeAttack.eState = EAttackState::ATTACKING;
                    activeAttack.fTimeInState = 0.0f;

                    pGameState->VecEvent.emplace(new CAttackStartEvent(pAttacker->GetId(), pTarget->GetId()));
                    break;
                }

                if(pNavigationComponent->m_vec3Destination != pTargetTransform->GetPosition()) {
                    pNavigationComponent->NavigateTo(pTargetTransform->GetPosition());
                }

                break;
            case EAttackState::ATTACKING:
                if(activeAttack.fTimeInState >= pAttackComp->fAttackPoint) {
                    pGameState->VecEvent.emplace(new CAttackHitEvent(pAttacker->GetId(), pTarget->GetId()));
                    activeAttack.eState = EAttackState::BACKSWING;
                    activeAttack.fTimeInState = 0.0f;
                }
                break;
            case EAttackState::BACKSWING:
                if (activeAttack.fTimeInState >= pAttackComp->fAttackTime - pAttackComp->fAttackPoint) {
                    activeAttack.eState = EAttackState::FINISHED;
                    pAttackComp->optCurrentAttack.value().fTimeInState = 0.0f;
                }
                break;
            case EAttackState::FINISHED:
                // TODO
                pGameState->VecEvent.emplace(new CAttackFinishedEvent(pAttacker->GetId()));
                pAttackComp->optCurrentAttack.value().eState = EAttackState::IDLE;
                pAttackComp->optCurrentAttack.value().fTimeInState = 0.0f;
                break;
                case EAttackState::CANCELLED:
                // TODO
                pGameState->VecEvent.emplace(new CAttackFinishedEvent(pAttacker->GetId()));
                pAttackComp->optCurrentAttack.reset(); // Done
                break;
            default:
                break;
        }
    }
}

void CAttackSystem::Finalize(CGameState* pGameState) {

}

void CAttackSystem::OnAttackIntention(CGameState* pGameState, CAttackIntentionEvent* pEvt) {
    CGameObject* pAttacker = pGameState->FindGameObjectById(pEvt->idUnit);
    CGameObject* pTarget = pGameState->FindGameObjectById(pEvt->idTarget);

    if(pAttacker == nullptr || pTarget == nullptr) {
        Logger::FormatErr("Invalid attack by unit %u on unit %u: at least one unit invalid", pEvt->idUnit, pEvt->idTarget);
        return;
    }

    CBasicAttackComponent* pAttackComp = pAttacker->GetComponent<CBasicAttackComponent>();

    if(pAttackComp == nullptr) {
        Logger::FormatErr("Invalid attack by unit %u on unit %u: attacker has no attack component", pEvt->idUnit, pEvt->idTarget);
        return;
    }

    if(pAttackComp->optCurrentAttack.has_value()) {
        ActiveAttack_t& atk = pAttackComp->optCurrentAttack.value();
        if(atk.idTarget == pEvt->idTarget) {
            return;
        }
    }

    ActiveAttack_t activeAttack {
        .idTarget = pEvt->idTarget,
        .eState = EAttackState::IDLE,
        .fTimeInState = 0.0f
    };
    pAttackComp->optCurrentAttack.emplace(activeAttack);
}

void CAttackSystem::OnAttackHit(CGameState* pGameState, CAttackHitEvent* pEvt) {
    CGameObject* pAttacker = pGameState->FindGameObjectById(pEvt->idAttacker);
    CGameObject* pTarget = pGameState->FindGameObjectById(pEvt->idTarget);

    if(pAttacker == nullptr || pTarget == nullptr) {
        Logger::FormatErr("Invalid attack by unit %u on unit %u: at least one unit invalid", pEvt->idAttacker, pEvt->idTarget);
        return;
    }

    ActiveAttack_t activeAttack {
        .idTarget = pEvt->idTarget,
        .eState = EAttackState::IDLE,
        .fTimeInState = 0.0f
    };

    CBasicAttackComponent* pAttackComp = pAttacker->GetComponent<CBasicAttackComponent>();

    if(pAttackComp == nullptr) {
        Logger::FormatErr("Invalid attack by unit %u on unit %u: attacker has no attack component", pEvt->idAttacker, pEvt->idTarget);
        return;
    }

    pGameState->VecEvent.emplace(new CDamageEvent(pEvt->idAttacker, pEvt->idTarget, 10));

}

void CAttackSystem::OnMoveIntention(CGameState* pGameState, CMoveIntentionEvent* pEvt) {
    // CBasicAttackComponent* pAttackComp = pGameState->FindGameObjectById(pEvt->idUnit)->GetComponent<CBasicAttackComponent>();
    // 
    // if(pAttackComp && pAttackComp->optCurrentAttack.has_value()) {
    //     ActiveAttack_t& atk = pAttackComp->optCurrentAttack.value();
    //     atk.eState = EAttackState::CANCELLED;
    // }
}

void CAttackSystem::OnSpellCastAttempt(CGameState* pGameState, CSpellAttemptCastEvent* pEvt) {
    CBasicAttackComponent* pAttackComp = pGameState->FindGameObjectById(pEvt->m_idCaster)->GetComponent<CBasicAttackComponent>();
    
    if(pAttackComp && pAttackComp->optCurrentAttack.has_value()) {
        ActiveAttack_t& atk = pAttackComp->optCurrentAttack.value();
        atk.eState = EAttackState::CANCELLED;
    }
}