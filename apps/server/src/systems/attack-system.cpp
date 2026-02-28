#include "systems/attack-system.h"

#include <game/server-game-state.h>

CAttackSystem::CAttackSystem() {
}

void CAttackSystem::Update(CServerGameState* pGameState, float fDelta) {
    for(CBasicAttackComponent& attackComp : pGameState->GetAllBasicAttack()) {
        if(!attackComp.optCurrentAttack.has_value()) {
            continue;
        }

        ActiveAttack_t& activeAttack = attackComp.optCurrentAttack.value();
        activeAttack.fTimeInState += fDelta;

        CTransformComponent* pAttackerTransform = nullptr;
        CTransformComponent* pTargetTransform = nullptr;
        CNavigationComponent* pNavigationComponent = nullptr;

        if(pGameState->GetHealth(attackComp.idUnit) && pGameState->GetHealth(attackComp.idUnit)->bIsDead) {
            attackComp.optCurrentAttack.reset();
            continue;
        }

        if(CHealthComponent* pHealthComp = pGameState->GetHealth(activeAttack.idTarget)) {
            if(pHealthComp->bIsDead) {
                attackComp.optCurrentAttack.reset();
                continue;
            }
        }

        switch(activeAttack.eState) {
            case EAttackState::IDLE:
            case EAttackState::APPROACHING:
                pAttackerTransform = pGameState->GetTransform(attackComp.idUnit);
                pTargetTransform = pGameState->GetTransform(activeAttack.idTarget);

                pNavigationComponent = pGameState->GetNavigation(attackComp.idUnit);
                if(attackComp.fRange > (pAttackerTransform->GetPosition() - pTargetTransform->GetPosition()).Length()) {
                    activeAttack.eState = EAttackState::ATTACKING;
                    activeAttack.fTimeInState = 0.0f;

                    pGameState->EmitEvent(new CAttackStartEvent(attackComp.idUnit, activeAttack.idTarget));
                    break;
                }

                if(pNavigationComponent->vec3Destination != pTargetTransform->GetPosition()) {
                    pNavigationComponent->vec3Destination = pTargetTransform->GetPosition();
                    pNavigationComponent->eStatus = ENavigationStatus::PATHING;
                }

                break;
            case EAttackState::ATTACKING:
                if(activeAttack.fTimeInState >= attackComp.fAttackPoint) {
                    pGameState->EmitEvent(new CAttackHitEvent(attackComp.idUnit, activeAttack.idTarget));
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
                pGameState->EmitEvent(new CAttackFinishedEvent(attackComp.idUnit));
                attackComp.optCurrentAttack.value().eState = EAttackState::IDLE;
                attackComp.optCurrentAttack.value().fTimeInState = 0.0f;
                break;
                case EAttackState::CANCELLED:
                // TODO
                pGameState->EmitEvent(new CAttackFinishedEvent(attackComp.idUnit));
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
    if(pEvt->idTarget == pEvt->idUnit) {
        Logger::FormatErr("Invalid attack by unit %u on unit %u: cannot attack self", pEvt->idUnit, pEvt->idTarget);
        return;
    }

    CBasicAttackComponent* attackComp = pGameState->GetBasicAttack(pEvt->idUnit);

    if(attackComp == nullptr) {
        Logger::FormatErr("Invalid attack by unit %u on unit %u: attacker has no attack component", pEvt->idUnit, pEvt->idTarget);
        return;
    }

    // Already attacking that target
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
    ActiveAttack_t activeAttack {
        .idTarget = pEvt->idTarget,
        .eState = EAttackState::IDLE,
        .fTimeInState = 0.0f
    };

    CBasicAttackComponent* attackComp = pGameState->GetBasicAttack(pEvt->idAttacker);

    if(attackComp == nullptr) {
        Logger::FormatErr("Invalid attack by unit %u on unit %u: attacker has no attack component", pEvt->idAttacker, pEvt->idTarget);
        return;
    }

    pGameState->EmitEvent(new CDamageEvent(pEvt->idAttacker, pEvt->idTarget, 10));

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