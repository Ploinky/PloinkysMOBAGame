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
        CTransformComponent* pTransform = pGameState->GetTransform(nav.idUnit);
        nav.pNavGridAgent->position = {pTransform->GetPosition().x, 0, pTransform->GetPosition().z};
        // Do not path dead units
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

        // Update nav components target if intent is to walk somewhere
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

        CMovementComponent* pMovement = pGameState->GetMovement(nav.idUnit);
        
        if(pTransform == nullptr || pMovement == nullptr) {
            continue;
        }
        
        // TODO Close enough, unit does not move
        if((nav.vec3Destination - pTransform->GetPosition()).Length() < 10) {
            continue;
        }

        // Trying to path, but no path currently set
        if(nav.pNavGridAgent->path.size() == 0) {
            Logger::FormatMsg("Planning initial path for %d to <%f, %f>", pTransform->idUnit, nav.vec3Destination.x, nav.vec3Destination.z);
            nav.pNavGridAgent->path = pNavMap->GetPath(nav.pNavGridAgent, {pTransform->GetPosition().x, pTransform->GetPosition().z}, {nav.vec3Destination.x, nav.vec3Destination.z});
            
            if(nav.pNavGridAgent->path.size() == 0) {
                // TODO this needs to be handled
                continue;
            }
        } else if(nav.vec3Destination.x != nav.pNavGridAgent->path.back().x && nav.vec3Destination.z != nav.pNavGridAgent->path.back().y) {
            Logger::FormatMsg("Changing destination, planning new path for %d to <%f, %f>", pTransform->idUnit, nav.vec3Destination.x, nav.vec3Destination.z);
            nav.pNavGridAgent->path = pNavMap->GetPath(nav.pNavGridAgent, {pTransform->GetPosition().x, pTransform->GetPosition().z}, {nav.vec3Destination.x, nav.vec3Destination.z});
            
            if(nav.pNavGridAgent->path.size() == 0) {
                // TODO this needs to be handled
                continue;
            }
        }
            // Already pathing, but to the wrong target!
        
        Vector2 vec2IntermediateTarget = nav.pNavGridAgent->path.at(0);
        Vector3 vec3IntermediateTarget = {vec2IntermediateTarget.x, 0, vec2IntermediateTarget.y};

        // Close enough to the current waypoint, so move on to the next one
        // TODO magic number
        if((vec3IntermediateTarget - pTransform->GetPosition()).Length() < 10) {
            Logger::FormatMsg("%d arrived at waypoint <%d, %d> on the way to <%f, %f>, moving on to the next waypoint",
                pTransform->idUnit, vec3IntermediateTarget.x, vec3IntermediateTarget.z, nav.vec3Destination.x, nav.vec3Destination.z);

            nav.pNavGridAgent->path.erase(nav.pNavGridAgent->path.begin());

            if(nav.pNavGridAgent->path.size() == 0) {
                continue;
            }
            vec2IntermediateTarget = nav.pNavGridAgent->path.at(0);
            vec3IntermediateTarget = {vec2IntermediateTarget.x, 0, vec2IntermediateTarget.y};
            pMovement->vec3Target = {nav.pNavGridAgent->path.at(0).x, 0, nav.pNavGridAgent->path.at(0).y};
            pGameState->VecEvent.emplace(new CMoveIntentionEvent(nav.idUnit, pMovement->vec3Target, 0));
            continue;
        }
        
        // Logger::FormatMsg("And we're planning again for some reason?");
        // nav.pNavGridAgent->path = pNavMap->GetPath(nav.pNavGridAgent, {pTransform->GetPosition().x, pTransform->GetPosition().z}, {nav.vec3Destination.x, nav.vec3Destination.z});
        
        if(nav.pNavGridAgent->path.size() == 0) {
            // TODO this needs to be handled
            continue;
        }

        if(pMovement->vec3Target.x == nav.pNavGridAgent->path.at(0).x && pMovement->vec3Target.z == nav.pNavGridAgent->path.at(0).y) {
            continue;
        }

        pMovement->vec3Target = {nav.pNavGridAgent->path.at(0).x, 0, nav.pNavGridAgent->path.at(0).y};
        pGameState->VecEvent.emplace(new CMoveIntentionEvent(nav.idUnit, pMovement->vec3Target, 0));
    }
}

void CNavigationSystem::OnSpellCastStart(CServerGameState* pGameState, CSpellCastStartEvent* pCastStartEvent) {
    CNavigationComponent* pNavComp = pGameState->GetNavigation(pCastStartEvent->pCtx->idCaster);
    CTransformComponent* pTransformComp = pGameState->GetTransform(pCastStartEvent->pCtx->idCaster);

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
    CNavigationComponent* pNavComp = pGameState->GetNavigation(pEvt->idAttacker);

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
    CNavigationComponent* pNavComp = pGameState->GetNavigation(pMoveAttemptEvent->idUnit);

    if(pNavComp == nullptr) {
        return;
    }

    pNavComp->vec3Destination = pMoveAttemptEvent->vec3Position;
    pNavComp->bIsNavigating = true;

    if(CSpellCastComponent* pSpellCastComp = pGameState->GetSpellCast(pMoveAttemptEvent->idUnit)) {
        pSpellCastComp->optCurrentCast.reset();
    }
    if(CBasicAttackComponent* pAtkComp = pGameState->GetBasicAttack(pMoveAttemptEvent->idUnit)) {
        pAtkComp->optCurrentAttack.reset();
    }
}
