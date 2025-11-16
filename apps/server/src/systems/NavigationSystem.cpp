#include "NavigationSystem.h"

#include "game/server-game-state.h"
#include "common/navigation.h"
#include "components/Components.h"
#include "events.h"

CNavigationSystem::CNavigationSystem(NavigationMap* pMap) {
    m_pMap = pMap;
    REGISTER_EVENT_HANDLER(CSpellCastStartEvent, OnSpellCastStart);
    REGISTER_EVENT_HANDLER(CMoveAttemptEvent, OnMoveAttempt);
    REGISTER_EVENT_HANDLER(CAttackStartEvent, OnAttackStart);
}

void CNavigationSystem::Update(CServerGameState* pGameState, float fDelta) {
    for(CNavigationComponent& nav : pGameState->GetAllNavigation()) {
        if(pGameState->GetHealth(nav.idUnit) && pGameState->GetHealth(nav.idUnit)->bIsDead) {
            nav.bIsNavigating = false;
            nav.vec3Destination = Vector3::ZERO;
            if(CMovementComponent* pMovement = pGameState->GetMovement(nav.idUnit)) {
                if(CTransformComponent* pTransform = pGameState->GetTransform(nav.idUnit)) {
                    pMovement->vec3Target = pTransform->GetPosition();
                }
            }
            continue;
        }

        CIntentComponent* pIntentComp = pGameState->GetIntent(nav.idUnit);
        if(pIntentComp != nullptr) {
            if(pIntentComp->eType == EIntentType::MOVE) {
                if(pIntentComp->vec3Target != nav.vec3Destination) {
                    nav.vec3Destination = pIntentComp->vec3Target;
                    nav.bIsNavigating = true;
                }
            }
        }

        if(!nav.bIsNavigating) {
            continue;
        }

        NavigationMap* pNavMap = pGameState->GetNavMap();

        CTransformComponent* pTransform = pGameState->GetTransform(nav.idUnit);
        CMovementComponent* pMovement = pGameState->GetMovement(nav.idUnit);

        if(pTransform == nullptr || pMovement == nullptr) {
            return;
        }
        
        if((nav.vec3Destination - pTransform->GetPosition()).Length() < 10) {
            return;
        }

        if(nav.pNavGridAgent->path.size() == 0) {
            nav.pNavGridAgent->path = pNavMap->GetPath(nav.pNavGridAgent, {pTransform->GetPosition().x, pTransform->GetPosition().z}, {nav.vec3Destination.x, nav.vec3Destination.z});
            
            if(nav.pNavGridAgent->path.size() == 0) {
                // TODO this needs to be handled
                return;
            }
        }

        Vector2 vec2IntermediateTarget = nav.pNavGridAgent->path.at(0);
        Vector3 vec3IntermediateTarget = {vec2IntermediateTarget.x, 0, vec2IntermediateTarget.y};

        // TODO magic number
        if((vec3IntermediateTarget - pTransform->GetPosition()).Length() < 10) {
            nav.pNavGridAgent->path.erase(nav.pNavGridAgent->path.begin());

            if(nav.pNavGridAgent->path.size() == 0) {
                return;
            }
            vec2IntermediateTarget = nav.pNavGridAgent->path.at(0);
            vec3IntermediateTarget = {vec2IntermediateTarget.x, 0, vec2IntermediateTarget.y};
            pMovement->vec3Target = {nav.pNavGridAgent->path.at(0).x, 0, nav.pNavGridAgent->path.at(0).y};
            pGameState->VecEvent.emplace(new CMoveIntentionEvent(nav.idUnit, pMovement->vec3Target, 0));
            return;
        }

        nav.pNavGridAgent->path = pNavMap->GetPath(nav.pNavGridAgent, {pTransform->GetPosition().x, pTransform->GetPosition().z}, {nav.vec3Destination.x, nav.vec3Destination.z});
        
        if(nav.pNavGridAgent->path.size() == 0) {
            // TODO this needs to be handled
            return;
        }

        pMovement->vec3Target = {nav.pNavGridAgent->path.at(0).x, 0, nav.pNavGridAgent->path.at(0).y};
        pGameState->VecEvent.emplace(new CMoveIntentionEvent(nav.idUnit, pMovement->vec3Target, 0));
    }
}

void CNavigationSystem::OnSpellCastStart(CServerGameState* pGameState, CSpellCastStartEvent* pCastStartEvent) {
    CGameObject* pGameObject = pGameState->FindGameObjectById(pCastStartEvent->pCtx->idCaster);

    CNavigationComponent* pNavComp = pGameState->GetNavigation(pGameObject->GetId());
    CTransformComponent* pTransformComp = pGameState->GetTransform(pGameObject->GetId());

    if(pNavComp == nullptr) {
        return;
    }

    pNavComp->bIsNavigating = false;
    pNavComp->vec3Destination = Vector3::ZERO;
    if(CMovementComponent* pMovement = pGameState->GetMovement(pNavComp->idUnit)) {
        if(CTransformComponent* pTransform = pGameState->GetTransform(pNavComp->idUnit)) {
            pMovement->vec3Target = pTransform->GetPosition();
        }
    }
}


void CNavigationSystem::OnAttackStart(CServerGameState* pGameState, CAttackStartEvent* pEvt) {
    CGameObject* pGameObject = pGameState->FindGameObjectById(pEvt->idAttacker);

    CNavigationComponent* pNavComp = pGameState->GetNavigation(pGameObject->GetId());

    if(pNavComp == nullptr) {
        return;
    }


    pNavComp->bIsNavigating = false;
    pNavComp->vec3Destination = Vector3::ZERO;
    if(CMovementComponent* pMovement = pGameState->GetMovement(pNavComp->idUnit)) {
        if(CTransformComponent* pTransform = pGameState->GetTransform(pNavComp->idUnit)) {
            pMovement->vec3Target = pTransform->GetPosition();
        }
    }
}
void CNavigationSystem::OnMoveAttempt(CServerGameState* pGameState, CMoveAttemptEvent* pMoveAttemptEvent) {
    CGameObject* pGameObject = pGameState->FindGameObjectById(pMoveAttemptEvent->idUnit);
    CNavigationComponent* pNavComp = pGameState->GetNavigation(pGameObject->GetId());

    if(pNavComp == nullptr) {
        return;
    }

    pNavComp->vec3Destination = pMoveAttemptEvent->vec3Position;
    pNavComp->bIsNavigating = true;

    if(CSpellCastComponent* pSpellCastComp = pGameState->GetSpellCast(pGameObject->GetId())) {
        pSpellCastComp->optCurrentCast.reset();
    }
    if(CBasicAttackComponent* pAtkComp = pGameState->GetBasicAttack(pGameObject->GetId())) {
        pAtkComp->optCurrentAttack.reset();
    }
}
