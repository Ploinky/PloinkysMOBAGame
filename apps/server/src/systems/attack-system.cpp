#include "systems/attack-system.h"

#include <game/server-game-state.h>

CAttackSystem::CAttackSystem() {
    REGISTER_EVENT_HANDLER(CAttackIntentionEvent, OnAttackIntention)
    REGISTER_EVENT_HANDLER(CAttackHitEvent, OnAttackHit)
    REGISTER_EVENT_HANDLER(CMoveIntentionEvent, OnMoveIntention)
    REGISTER_EVENT_HANDLER(CSpellAttemptCastEvent, OnSpellCastAttempt)
}

void CAttackSystem::Update(CServerGameState* pGameState, float fDelta) {
    for(CBasicAttackComponent& attackComp : pGameState->GetAllBasicAttack()) {
        if(!attackComp.optCurrentAttack.has_value()) {
            continue;
        }

        ActiveAttack_t& activeAttack = attackComp.optCurrentAttack.value();
        activeAttack.fTimeInState += fDelta;

        CGameObject* pAttacker = pGameState->FindGameObjectById(attackComp.idUnit);
        CGameObject* pTarget = pGameState->FindGameObjectById(activeAttack.idTarget);
        CTransformComponent* pAttackerTransform = nullptr;
        CTransformComponent* pTargetTransform = nullptr;
        CNavigationComponent* pNavigationComponent = nullptr;

        if(pTarget == nullptr
            || pAttacker == nullptr
            || (pGameState->GetHealth(pAttacker->GetId()) && pGameState->GetHealth(pAttacker->GetId())->bIsDead)) {
            attackComp.optCurrentAttack.reset();
            continue;
        }

        if(CHealthComponent* pHealthComp = pGameState->GetHealth(pTarget->GetId())) {
            if(pHealthComp->bIsDead) {
                attackComp.optCurrentAttack.reset();
                continue;
            }
        }

        switch(activeAttack.eState) {
            case EAttackState::IDLE:
            case EAttackState::APPROACHING:
                pAttackerTransform = pGameState->GetTransform(pAttacker->GetId());
                pTargetTransform = pGameState->GetTransform(pTarget->GetId());

                pNavigationComponent = pGameState->GetNavigation(pAttacker->GetId());
                if(attackComp.fRange > (pAttackerTransform->GetPosition() - pTargetTransform->GetPosition()).Length()) {
                    activeAttack.eState = EAttackState::ATTACKING;
                    activeAttack.fTimeInState = 0.0f;

                    pGameState->VecEvent.emplace(new CAttackStartEvent(pAttacker->GetId(), pTarget->GetId()));
                    break;
                }

                if(pNavigationComponent->vec3Destination != pTargetTransform->GetPosition()) {
                    pNavigationComponent->vec3Destination = pTargetTransform->GetPosition();
                    pNavigationComponent->bIsNavigating = true;
                }

                break;
            case EAttackState::ATTACKING:
                if(activeAttack.fTimeInState >= attackComp.fAttackPoint) {
                    pGameState->VecEvent.emplace(new CAttackHitEvent(pAttacker->GetId(), pTarget->GetId()));
                    activeAttack.eState = EAttackState::BACKSWING;
                    activeAttack.fTimeInState = 0.0f;
                }
                break;
            case EAttackState::BACKSWING:
                if (activeAttack.fTimeInState >= attackComp.fAttackTime - attackComp.fAttackPoint) {
                    activeAttack.eState = EAttackState::FINISHED;
                    attackComp.optCurrentAttack.value().fTimeInState = 0.0f;
                }
                break;
            case EAttackState::FINISHED:
                // TODO
                pGameState->VecEvent.emplace(new CAttackFinishedEvent(pAttacker->GetId()));
                attackComp.optCurrentAttack.value().eState = EAttackState::IDLE;
                attackComp.optCurrentAttack.value().fTimeInState = 0.0f;
                break;
                case EAttackState::CANCELLED:
                // TODO
                pGameState->VecEvent.emplace(new CAttackFinishedEvent(pAttacker->GetId()));
                attackComp.optCurrentAttack.reset(); // Done
                break;
            default:
                break;
        }
    }
}

void CAttackSystem::Finalize(CServerGameState* pGameState) {

}

void CAttackSystem::OnAttackIntention(CServerGameState* pGameState, CAttackIntentionEvent* pEvt) {
    CGameObject* pAttacker = pGameState->FindGameObjectById(pEvt->idUnit);
    CGameObject* pTarget = pGameState->FindGameObjectById(pEvt->idTarget);

    if(pAttacker == nullptr || pTarget == nullptr) {
        Logger::FormatErr("Invalid attack by unit %u on unit %u: at least one unit invalid", pEvt->idUnit, pEvt->idTarget);
        return;
    }

    CBasicAttackComponent* attackComp = pGameState->GetBasicAttack(pAttacker->GetId());

    if(attackComp == nullptr) {
        Logger::FormatErr("Invalid attack by unit %u on unit %u: attacker has no attack component", pEvt->idUnit, pEvt->idTarget);
        return;
    }

    if(attackComp->optCurrentAttack.has_value()) {
        ActiveAttack_t& atk = attackComp->optCurrentAttack.value();
        if(atk.idTarget == pEvt->idTarget) {
            return;
        }
    }

    ActiveAttack_t activeAttack {
        .idTarget = pEvt->idTarget,
        .eState = EAttackState::IDLE,
        .fTimeInState = 0.0f
    };
    attackComp->optCurrentAttack.emplace(activeAttack);
}

void CAttackSystem::OnAttackHit(CServerGameState* pGameState, CAttackHitEvent* pEvt) {
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

    CBasicAttackComponent* attackComp = pGameState->GetBasicAttack(pAttacker->GetId());

    if(attackComp == nullptr) {
        Logger::FormatErr("Invalid attack by unit %u on unit %u: attacker has no attack component", pEvt->idAttacker, pEvt->idTarget);
        return;
    }

    pGameState->VecEvent.emplace(new CDamageEvent(pEvt->idAttacker, pEvt->idTarget, 10));

}

void CAttackSystem::OnMoveIntention(CServerGameState* pGameState, CMoveIntentionEvent* pEvt) {
    // CBasicAttackComponent* attackComp = pGameState->FindGameObjectById(pEvt->idUnit)->GetComponent<CBasicAttackComponent>();
    // 
    // if(attackComp && attackComp.optCurrentAttack.has_value()) {
    //     ActiveAttack_t& atk = attackComp.optCurrentAttack.value();
    //     atk.eState = EAttackState::CANCELLED;
    // }
}

void CAttackSystem::OnSpellCastAttempt(CServerGameState* pGameState, CSpellAttemptCastEvent* pEvt) {
    CBasicAttackComponent* attackComp = pGameState->GetBasicAttack(pEvt->m_idCaster);
    
    if(attackComp->optCurrentAttack.has_value()) {
        ActiveAttack_t& atk = attackComp->optCurrentAttack.value();
        atk.eState = EAttackState::CANCELLED;
    }
}