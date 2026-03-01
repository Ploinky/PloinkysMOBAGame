#include "NavigationSystem.h"

#include "game/server-game-state.h"
#include "common/navigation.h"
#include "components/Components.h"
#include "events.h"

#define GOAL_THRESHOLD 10
#define WAIT_BLOCKED 250

CNavigationSystem::CNavigationSystem(NavigationMap* pMap) {
    m_pMap = pMap;
}

void CNavigationSystem::Update(CServerGameState* pGameState, float fDelta) {
    for(auto& [id, nav] : pGameState->GetAllNavigation()) {
        UpdateEntity(pGameState, fDelta, nav, id);
    }
}

void CNavigationSystem::UpdateEntity(CServerGameState* pGameState, float fDelta, NavigationComponent_t& nav, UnitId id) {
    TransformComponent_t* pTransform = pGameState->GetTransform(id);
    nav.pNavGridAgent->position = {pTransform->GetPosition().x, pTransform->GetPosition().z};

    // Update nav components target if intent is to walk somewhere
    if(IntentComponent_t* pIntentComp = pGameState->GetIntent(id)) {
        if(pIntentComp->eType == EIntentType::MOVE) {
            if(nav.eStatus == ENavigationStatus::ARRIVED) {
                nav.eStatus = ENavigationStatus::IDLE;
                pIntentComp->eType = EIntentType::NONE;
            } else if(pIntentComp->vec3Target != nav.vec3Destination) {
                nav.vec3Destination = pIntentComp->vec3Target;
                nav.eStatus = ENavigationStatus::PATHING;
            }
        }
    }

    if(nav.eStatus == ENavigationStatus::IDLE) {
        return;
    }

    NavigationMap* pNavMap = pGameState->GetNavMap();

    MovementComponent_t* pMovement = pGameState->GetMovement(id);
    
    if(pTransform == nullptr || pMovement == nullptr) {
        return;
    }
    
    // close enough to target, entity is done moving
    // TODO does anybody else need to know?
    if((nav.vec3Destination - pTransform->GetPosition()).Length() < GOAL_THRESHOLD) {
        nav.eStatus = ENavigationStatus::IDLE;
        nav.vec3Destination = pTransform->GetPosition();
        return;
    }

    // Trying to path, but no path currently set
    if(nav.pNavGridAgent->path.size() == 0) {
        Logger::FormatMsg("Planning initial path for %d to <%f, %f>", id, nav.vec3Destination.x, nav.vec3Destination.z);
        nav.pNavGridAgent->path = pNavMap->GetPath(nav.pNavGridAgent, {pTransform->GetPosition().x, pTransform->GetPosition().z}, {nav.vec3Destination.x, nav.vec3Destination.z});
        
        if(nav.pNavGridAgent->path.size() == 0) {
            pGameState->EmitEvent(new CNavDestEvent(id, nav.vec3Destination));
            nav.eStatus = ENavigationStatus::ARRIVED;
            nav.vec3Destination = {pTransform->GetPosition().x, 0, pTransform->GetPosition().z};
            // TODO this needs to be handled
            return;
        }
    } else if(nav.vec3Destination.x != nav.pNavGridAgent->path.back().x && nav.vec3Destination.z != nav.pNavGridAgent->path.back().y) {
        Logger::FormatMsg("Changing destination, planning new path for %d to <%f, %f>", id, nav.vec3Destination.x, nav.vec3Destination.z);
        nav.pNavGridAgent->path = pNavMap->GetPath(nav.pNavGridAgent, {pTransform->GetPosition().x, pTransform->GetPosition().z}, {nav.vec3Destination.x, nav.vec3Destination.z});
        
        if(nav.pNavGridAgent->path.size() == 0) {
            pGameState->EmitEvent(new CNavDestEvent(id, nav.vec3Destination));
            nav.eStatus = ENavigationStatus::ARRIVED;
            nav.vec3Destination = {pTransform->GetPosition().x, 0, pTransform->GetPosition().z};
            // TODO this needs to be handled
            return;
        }
    } else if (nav.eStatus == ENavigationStatus::BLOCKED) {
        nav.fTimeBlocked += fDelta;
        if(nav.fTimeBlocked < WAIT_BLOCKED) {
            Logger::FormatMsg("Entity %d blocked, waiting for renav", id);
            return;
        }
        // OH NO!
        Logger::FormatMsg("Entity %d blocked on its way to <%f, %f>, needs renav",
            id, nav.vec3Destination.x, nav.vec3Destination.z);
        nav.pNavGridAgent->path = pNavMap->GetPath(nav.pNavGridAgent, {pTransform->GetPosition().x, pTransform->GetPosition().z}, {nav.vec3Destination.x, nav.vec3Destination.z});
        
        if(nav.pNavGridAgent->path.size() == 0) {
            pGameState->EmitEvent(new CNavDestEvent(id, nav.vec3Destination));
            nav.eStatus = ENavigationStatus::ARRIVED;
            nav.vec3Destination = {pTransform->GetPosition().x, 0, pTransform->GetPosition().z};
            return;
        } else {
            nav.eStatus = ENavigationStatus::PATHING;
        }
    }

    Vector2 vec2IntermediateTarget = nav.pNavGridAgent->path.at(0);
    Vector3 vec3IntermediateTarget = {vec2IntermediateTarget.x, 0, vec2IntermediateTarget.y};

    // Close enough to the current waypoint, so move on to the next one
    // TODO magic number
    if((vec3IntermediateTarget - pTransform->GetPosition()).Length() < GOAL_THRESHOLD) {
        Logger::FormatMsg("%d arrived at waypoint <%d, %d> on the way to <%f, %f>, moving on to the next waypoint",
            id, vec3IntermediateTarget.x, vec3IntermediateTarget.z, nav.vec3Destination.x, nav.vec3Destination.z);

        nav.pNavGridAgent->path.erase(nav.pNavGridAgent->path.begin());

        if(nav.pNavGridAgent->path.size() == 0) {
            return;
        }
        vec2IntermediateTarget = nav.pNavGridAgent->path.at(0);
        vec3IntermediateTarget = {vec2IntermediateTarget.x, 0, vec2IntermediateTarget.y};
        pMovement->vec3Target = {nav.pNavGridAgent->path.at(0).x, 0, nav.pNavGridAgent->path.at(0).y};
        pGameState->EmitEvent(new CMoveIntentionEvent(id, pMovement->vec3Target, 0));
        return;
    }
    
    if(nav.pNavGridAgent->path.size() == 0) {
        // TODO this needs to be handled
        return;
    }

    if(pMovement->vec3Target.x == nav.pNavGridAgent->path.at(0).x && pMovement->vec3Target.z == nav.pNavGridAgent->path.at(0).y) {
        return;
    }

    pMovement->vec3Target = {nav.pNavGridAgent->path.at(0).x, 0, nav.pNavGridAgent->path.at(0).y};
    pGameState->EmitEvent(new CMoveIntentionEvent(id, pMovement->vec3Target, 0));
}

void CNavigationSystem::OnSpellCastStart(CServerGameState* pGameState, CSpellCastStartEvent* pCastStartEvent) {
    NavigationComponent_t* pNavComp = pGameState->GetNavigation(pCastStartEvent->pCtx->idCaster);
    TransformComponent_t* pTransformComp = pGameState->GetTransform(pCastStartEvent->pCtx->idCaster);

    if(pNavComp == nullptr) {
        return;
    }

    pNavComp->eStatus = ENavigationStatus::IDLE;
    pNavComp->vec3Destination = Vector3::ZERO;
    if(MovementComponent_t* pMovement = pGameState->GetMovement(pCastStartEvent->pCtx->idCaster)) {
        if(TransformComponent_t* pTransform = pGameState->GetTransform(pCastStartEvent->pCtx->idCaster)) {
            pMovement->vec3Target = pTransform->GetPosition();
        }
    }
}


void CNavigationSystem::OnAttackStart(CServerGameState* pGameState, CAttackStartEvent* pEvt) {
    NavigationComponent_t* pNavComp = pGameState->GetNavigation(pEvt->idAttacker);

    if(pNavComp == nullptr) {
        return;
    }


    pNavComp->eStatus = ENavigationStatus::IDLE;
    pNavComp->vec3Destination = Vector3::ZERO;
    if(MovementComponent_t* pMovement = pGameState->GetMovement(pEvt->idAttacker)) {
        if(TransformComponent_t* pTransform = pGameState->GetTransform(pEvt->idAttacker)) {
            pMovement->vec3Target = pTransform->GetPosition();
        }
    }
}
void CNavigationSystem::OnMoveAttempt(CServerGameState* pGameState, CMoveAttemptEvent* pMoveAttemptEvent) {
    NavigationComponent_t* pNavComp = pGameState->GetNavigation(pMoveAttemptEvent->idUnit);

    if(pNavComp == nullptr) {
        return;
    }

    pNavComp->vec3Destination = pMoveAttemptEvent->vec3Position;
    pNavComp->eStatus = ENavigationStatus::PATHING;

    if(SpellCastComponent_t* pSpellCastComp = pGameState->GetSpellCast(pMoveAttemptEvent->idUnit)) {
        pSpellCastComp->optCurrentCast.reset();
    }
    if(BasicAttackComponent_t* pAtkComp = pGameState->GetBasicAttack(pMoveAttemptEvent->idUnit)) {
        pAtkComp->optCurrentAttack.reset();
    }
}

void CNavigationSystem::OnDeath(CServerGameState* pGameState, CDeathEvent* pEvt) {
    NavigationComponent_t* pNavComp = pGameState->GetNavigation(pEvt->idTarget);

    if(pNavComp == nullptr) {
        return;
    }

    pNavComp->eStatus = ENavigationStatus::IDLE;
    pNavComp->vec3Destination = Vector3::ZERO;
}
