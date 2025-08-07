#include "NavigationSystem.h"

#include "GameState.h"
#include "Common/navigation.h"
#include "components/Components.h"
#include "events.h"

CNavigationSystem::CNavigationSystem(NavigationMap* pMap) {
    m_pMap = pMap;
    REGISTER_EVENT_HANDLER(CSpellCastStartEvent, OnSpellCastStart);
    REGISTER_EVENT_HANDLER(CMoveAttemptEvent, OnMoveAttempt);
}

void CNavigationSystem::Update(CGameState* pGameState, float fDelta) {
    for(std::pair<UnitId, CGameObject*> goPair : pGameState->GameObjects) {
        CGameObject* pGameObject = goPair.second;

        CNavigationComponent* pNavComp = pGameObject->GetComponent<CNavigationComponent>();

        if(pNavComp == nullptr || !pNavComp->m_bIsNavigating) {
            continue;
        }

        NavigationMap* pNavMap = pGameState->GetNavMap();

        CTransformComponent* pTransform = pGameObject->GetComponent<CTransformComponent>();
        CMovementComponent* pMovement = pGameObject->GetComponent<CMovementComponent>();

        if(pTransform == nullptr || pMovement == nullptr) {
            return;
        }
        
        if((pNavComp->m_vec3Destination - pTransform->GetPosition()).Length() < 10) {
            return;
        }

        if(pNavComp->m_pNavGridAgent->path.size() == 0) {
            pNavComp->m_pNavGridAgent->path = pNavMap->GetPath(pNavComp->m_pNavGridAgent, {pTransform->GetPosition().x, pTransform->GetPosition().z}, {pNavComp->m_vec3Destination.x, pNavComp->m_vec3Destination.z});
            
            if(pNavComp->m_pNavGridAgent->path.size() == 0) {
                // TODO this needs to be handled
                return;
            }
        }

        Vector2 vec2IntermediateTarget = pNavComp->m_pNavGridAgent->path.at(0);
        Vector3 vec3IntermediateTarget = {vec2IntermediateTarget.x, 0, vec2IntermediateTarget.y};

        // TODO magic number
        if((vec3IntermediateTarget - pTransform->GetPosition()).Length() < 10) {
            pNavComp->m_pNavGridAgent->path.erase(pNavComp->m_pNavGridAgent->path.begin());

            if(pNavComp->m_pNavGridAgent->path.size() == 0) {
                return;
            }
            vec2IntermediateTarget = pNavComp->m_pNavGridAgent->path.at(0);
            vec3IntermediateTarget = {vec2IntermediateTarget.x, 0, vec2IntermediateTarget.y};
            pMovement->SetTarget({pNavComp->m_pNavGridAgent->path.at(0).x, 0, pNavComp->m_pNavGridAgent->path.at(0).y});
            pGameState->VecEvent.emplace(new CMoveIntentionEvent(goPair.first, pMovement->GetTarget(), 0));
            return;
        }

        pNavComp->m_pNavGridAgent->path = pNavMap->GetPath(pNavComp->m_pNavGridAgent, {pTransform->GetPosition().x, pTransform->GetPosition().z}, {pNavComp->m_vec3Destination.x, pNavComp->m_vec3Destination.z});
        
        if(pNavComp->m_pNavGridAgent->path.size() == 0) {
            // TODO this needs to be handled
            return;
        }

        pMovement->SetTarget({pNavComp->m_pNavGridAgent->path.at(0).x, 0, pNavComp->m_pNavGridAgent->path.at(0).y});
        pGameState->VecEvent.emplace(new CMoveIntentionEvent(goPair.first, pMovement->GetTarget(), 0));
    }
}

void CNavigationSystem::OnSpellCastStart(CGameState* pGameState, CSpellCastStartEvent* pCastStartEvent) {
    CGameObject* pGameObject = pGameState->FindGameObjectById(pCastStartEvent->pCtx->idCaster);

    CNavigationComponent* pNavComp = pGameObject->GetComponent<CNavigationComponent>();
    CTransformComponent* pTransformComp = pGameObject->GetComponent<CTransformComponent>();

    if(pNavComp == nullptr) {
        return;
    }

    pNavComp->StopNavigation();
    // pGameState->VecEvent.emplace(new CMoveIntentionEvent(pGameObject->GetId(), pTransformComp->GetPosition(), 0));
}

void CNavigationSystem::OnMoveAttempt(CGameState* pGameState, CMoveAttemptEvent* pMoveAttemptEvent) {
    CGameObject* pGameObject = pGameState->FindGameObjectById(pMoveAttemptEvent->idUnit);
    CNavigationComponent* pNavComp = pGameObject->GetComponent<CNavigationComponent>();

    if(pNavComp == nullptr) {
        return;
    }

    pNavComp->NavigateTo(pMoveAttemptEvent->vec3Position);

    if(CSpellCastComponent* pSpellCastComp = pGameObject->GetComponent<CSpellCastComponent>()) {
        pSpellCastComp->optCurrentCast.reset();
    }
}
