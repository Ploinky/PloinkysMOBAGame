#include "attack-system.h"

#include "../game/server-game-state.h"

CAttackSystem::CAttackSystem() {
}

void CAttackSystem::Update(CServerGameState* pGameState, float fDelta) {
    for(auto& [id, attackComp] : pGameState->GetAllBasicAttack()) {
        if(!attackComp.optCurrentAttack.has_value()) {
            continue;
        }

        ActiveAttack_t& activeAttack = attackComp.optCurrentAttack.value();
        activeAttack.fTimeInState += fDelta;

        TransformComponent_t* pAttackerTransform = nullptr;
        TransformComponent_t* pTargetTransform = nullptr;
        NavigationComponent_t* pNavigationComponent = nullptr;

        if(pGameState->GetHealth(id) && pGameState->GetHealth(id)->bIsDead) {
            attackComp.optCurrentAttack.reset();
            continue;
        }

        if(HealthComponent_t* pHealthComp = pGameState->GetHealth(activeAttack.idTarget)) {
            if(pHealthComp->bIsDead) {
                attackComp.optCurrentAttack.reset();
                pGameState->EmitEvent(CAttackFinishedEvent(id));
                continue;
            }
        }

        switch(activeAttack.eState) {
            case EAttackState::IDLE:
            case EAttackState::APPROACHING:
                pAttackerTransform = pGameState->GetTransform(id);
                pTargetTransform = pGameState->GetTransform(activeAttack.idTarget);

                pNavigationComponent = pGameState->GetNavigation(id);
                if(attackComp.fRange > (pAttackerTransform->GetPosition() - pTargetTransform->GetPosition()).Length()) {
                    activeAttack.eState = EAttackState::ATTACKING;
                    activeAttack.fTimeInState = 0.0f;

                    pGameState->EmitEvent(CAttackStartEvent(id, activeAttack.idTarget));
                    break;
                }

                if(pNavigationComponent->vec3Destination != pTargetTransform->GetPosition()) {
                    pNavigationComponent->vec3Destination = pTargetTransform->GetPosition();
                    pNavigationComponent->eStatus = ENavigationStatus::PATHING;
                }

                break;
            case EAttackState::ATTACKING:
                if(activeAttack.fTimeInState >= attackComp.fAttackPoint) {
                    pGameState->EmitEvent(CAttackHitEvent(id, activeAttack.idTarget));
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
                pGameState->EmitEvent(CAttackFinishedEvent(id));
                attackComp.optCurrentAttack.value().eState = EAttackState::IDLE;
                attackComp.optCurrentAttack.value().fTimeInState = 0.0f;
                break;
                case EAttackState::CANCELLED:
                // TODO
                pGameState->EmitEvent(CAttackFinishedEvent(id));
                attackComp.optCurrentAttack.reset(); // Done
                break;
            default:
                break;
        }
    }
}

void CAttackSystem::Finalize(CServerGameState* pGameState) {

}

void CAttackSystem::OnAttackIntention(CServerGameState* pGameState, const CAttackIntentionEvent& evt) {
    if(evt.idTarget == evt.idUnit) {
        Logger::FormatErr("Invalid attack by unit %u on unit %u: cannot attack self", evt.idUnit, evt.idTarget);
        return;
    }

    BasicAttackComponent_t* attackComp = pGameState->GetBasicAttack(evt.idUnit);

    if(attackComp == nullptr) {
        Logger::FormatErr("Invalid attack by unit %u on unit %u: attacker has no attack component", evt.idUnit, evt.idTarget);
        return;
    }

    // Already attacking that target
    if(attackComp->optCurrentAttack.has_value()) {
        ActiveAttack_t& atk = attackComp->optCurrentAttack.value();
        if(atk.idTarget == evt.idTarget) {
            return;
        }
    }

    ActiveAttack_t activeAttack {
        .idTarget = evt.idTarget,
        .eState = EAttackState::IDLE,
        .fTimeInState = 0.0f
    };
    attackComp->optCurrentAttack.emplace(activeAttack);
}

void CAttackSystem::OnAttackHit(CServerGameState* pGameState, const CAttackHitEvent& evt) {
    ActiveAttack_t activeAttack {
        .idTarget = evt.idTarget,
        .eState = EAttackState::IDLE,
        .fTimeInState = 0.0f
    };

    BasicAttackComponent_t* attackComp = pGameState->GetBasicAttack(evt.idAttacker);

    if(attackComp == nullptr) {
        Logger::FormatErr("Invalid attack by unit %u on unit %u: attacker has no attack component", evt.idAttacker, evt.idTarget);
        return;
    }

    pGameState->EmitEvent(CDamageEvent(evt.idAttacker, evt.idTarget, 10));

}

void CAttackSystem::OnMoveIntention(CServerGameState* pGameState, const CMoveIntentionEvent& evt) {
    // CBasicAttackComponent* attackComp = pGameState->FindGameObjectById(pEvt->idUnit)->GetComponent<CBasicAttackComponent>();
    // 
    // if(attackComp && attackComp.optCurrentAttack.has_value()) {
    //     ActiveAttack_t& atk = attackComp.optCurrentAttack.value();
    //     atk.eState = EAttackState::CANCELLED;
    // }
}

void CAttackSystem::OnSpellCastAttempt(CServerGameState* pGameState, const CSpellAttemptCastEvent& evt) {
    BasicAttackComponent_t* attackComp = pGameState->GetBasicAttack(evt.m_idCaster);
    
    if(attackComp->optCurrentAttack.has_value()) {
        ActiveAttack_t& atk = attackComp->optCurrentAttack.value();
        atk.eState = EAttackState::CANCELLED;
    }
}

void CAttackSystem::OnUnitDeath(CServerGameState* pGameState, const CDeathEvent& evt) {
    for (auto& [id, attack] : pGameState->GetAllBasicAttack()) {
        if (!attack.optCurrentAttack.has_value()) {
            continue;
        }

        if (attack.optCurrentAttack.value().idTarget != evt.idTarget) {
            continue;
        }

        attack.optCurrentAttack.reset();
        pGameState->EmitEvent(CAttackFinishedEvent(id));
    }
}