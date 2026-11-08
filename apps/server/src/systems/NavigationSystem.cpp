#include "NavigationSystem.h"

#include "../game/server-game-state.h"
#include "common/navigation.h"
#include "../components/Components.h"
#include "../events.h"

#define GOAL_THRESHOLD 10
#define WAIT_BLOCKED 50

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
    if(nav.path.size() == 0) {
        Logger::FormatMsg("Planning initial path for %d to <%f, %f>", id, nav.vec3Destination.x, nav.vec3Destination.z);
        TryPath(pGameState, id);
        if (nav.path.empty()) {
            return;
        }
    } else if(nav.vec3Destination.x != nav.path.back().vec2To.x || nav.vec3Destination.z != nav.path.back().vec2To.y) {
        Logger::FormatMsg("Changing destination, planning new path for %d to <%f, %f>", id, nav.vec3Destination.x, nav.vec3Destination.z);
        TryPath(pGameState, id);
        if (nav.path.empty()) {
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
        TryPath(pGameState, id);
        if (nav.path.empty()) {
            return;
        }
    }

    PathSegment_t segment = nav.path.at(0);
    Vector3 vec3IntermediateTarget = {segment.vec2To.x, 0, segment.vec2To.y};

    // Close enough to the current waypoint, so move on to the next one
    // TODO magic number
    if((vec3IntermediateTarget - pTransform->GetPosition()).Length() < GOAL_THRESHOLD) {
        Logger::FormatMsg("%d arrived at waypoint <%f, %f> on the way to <%f, %f>, moving on to the next waypoint",
            id, vec3IntermediateTarget.x, vec3IntermediateTarget.z, nav.vec3Destination.x, nav.vec3Destination.z);

        nav.path.erase(nav.path.begin());

        if(nav.path.size() == 0) {
            return;
        }
        segment = nav.path.at(0);
        vec3IntermediateTarget = {segment.vec2To.x, 0, segment.vec2To.y};
        pMovement->vec3Target = {nav.path.at(0).vec2To.x, 0, nav.path.at(0).vec2To.y};
        pGameState->EmitEvent(CMoveIntentionEvent(id, pMovement->vec3Target, 0));
        return;
    }
    
    if(nav.path.empty()) {
        // TODO this needs to be handled
        return;
    }

    if(pMovement->vec3Target.x == nav.path.at(0).vec2To.x && pMovement->vec3Target.z == nav.path.at(0).vec2To.y) {
        return;
    }

    pMovement->vec3Target = {nav.path.at(0).vec2To.x, 0, nav.path.at(0).vec2To.y};
    pGameState->EmitEvent(CMoveIntentionEvent(id, pMovement->vec3Target, 0));
}

void CNavigationSystem::TryPath(CServerGameState* pGameState, UnitId id) {
    RemoveBlocker(pGameState, id);

    TransformComponent_t* pTransform = pGameState->GetTransform(id);
    NavigationComponent_t* pNav = pGameState->GetNavigation(id);

    PathResult_t pathResult = m_pMap->RequestPath({pTransform->GetPosition().x, pTransform->GetPosition().z}, {pNav->vec3Destination.x, pNav->vec3Destination.z}, pNav->nCollisionRadius);
    
    if (pathResult.eResultType == EPathResultType::UNREACHABLE || pathResult.eResultType == EPathResultType::OCCUPIED) {
        Vector2 vec2NextFree = m_pMap->ClosestFreePosition({pNav->vec3Destination.x, pNav->vec3Destination.z});
        pNav->vec3Destination.x = vec2NextFree.x;
        pNav->vec3Destination.z = vec2NextFree.y;
        pathResult = m_pMap->RequestPath({pTransform->GetPosition().x, pTransform->GetPosition().z}, {pNav->vec3Destination.x, pNav->vec3Destination.z}, pNav->nCollisionRadius);
        
        
        // TOOD PLS NO STOP
        if(IntentComponent_t* pIntentComp = pGameState->GetIntent(id)) {
            if(pIntentComp->eType == EIntentType::MOVE) {
                if(pIntentComp->vec3Target != pNav->vec3Destination) {
                    pIntentComp->vec3Target = pNav->vec3Destination;
                    pNav->eStatus = ENavigationStatus::PATHING;
                }
            }
        }
    }

    if (pathResult.eResultType != EPathResultType::FOUND) {
        Logger::FormatMsg("PATHING FAIL!!!");
    }
    
    pNav->path = pathResult.vecVec2Path;

    if(pNav->path.empty()) {
        pGameState->EmitEvent(CNavDestEvent(id, pNav->vec3Destination));
        pNav->eStatus = ENavigationStatus::ARRIVED;
        pNav->vec3Destination = {pTransform->GetPosition().x, 0, pTransform->GetPosition().z};
        AddBlocker(pGameState, id);
        return;
    } else {
        pNav->eStatus = ENavigationStatus::PATHING;
    }
}

void CNavigationSystem::AddBlocker(CServerGameState* pGameState, UnitId id) {
    NavigationComponent_t* pNavComp = pGameState->GetNavigation(id);
    TransformComponent_t* pTransform = pGameState->GetTransform(id);

    if(pNavComp == nullptr || pTransform == nullptr) {
        return;
    }

    if (pNavComp->hBlocker != INVALID_HANDLE) {
        m_pMap->RemoveBlocker(pNavComp->hBlocker);
    }

    pNavComp->hBlocker = m_pMap->AddBlocker(pTransform->GetPosition().x, pTransform->GetPosition().z, pNavComp->nCollisionRadius);
}

void CNavigationSystem::RemoveBlocker(CServerGameState* pGameState, UnitId id) {
    NavigationComponent_t* pNavComp = pGameState->GetNavigation(id);

    if(pNavComp == nullptr || pNavComp->hBlocker == INVALID_HANDLE) {
        return;
    }

    m_pMap->RemoveBlocker(pNavComp->hBlocker);
    pNavComp->hBlocker = INVALID_HANDLE;
}

void CNavigationSystem::OnSpellCastStart(CServerGameState* pGameState, const CSpellCastStartEvent& evt) {
    NavigationComponent_t* pNavComp = pGameState->GetNavigation(evt.pCtx->idCaster);
    TransformComponent_t* pTransformComp = pGameState->GetTransform(evt.pCtx->idCaster);

    if(pNavComp == nullptr) {
        return;
    }
    
    pNavComp->eStatus = ENavigationStatus::IDLE;
    pNavComp->vec3Destination = Vector3::ZERO;
    AddBlocker(pGameState, evt.pCtx->idCaster);
    if(MovementComponent_t* pMovement = pGameState->GetMovement(evt.pCtx->idCaster)) {
        if(TransformComponent_t* pTransform = pGameState->GetTransform(evt.pCtx->idCaster)) {
            pMovement->vec3Target = pTransform->GetPosition();
        }
    }
}


void CNavigationSystem::OnAttackStart(CServerGameState* pGameState, const CAttackStartEvent& evt) {
    NavigationComponent_t* pNavComp = pGameState->GetNavigation(evt.idAttacker);

    if(pNavComp == nullptr) {
        return;
    }


    pNavComp->eStatus = ENavigationStatus::IDLE;
    pNavComp->vec3Destination = Vector3::ZERO;
    
    AddBlocker(pGameState, evt.idAttacker);

    if(MovementComponent_t* pMovement = pGameState->GetMovement(evt.idAttacker)) {
        if(TransformComponent_t* pTransform = pGameState->GetTransform(evt.idAttacker)) {
            pMovement->vec3Target = pTransform->GetPosition();
        }
    }
}
void CNavigationSystem::OnUnitIdle(CServerGameState* pGameState, const CEntityIdleEvent& evt) {
    AddBlocker(pGameState, evt.idTarget);
}

void CNavigationSystem::OnMoveAttempt(CServerGameState* pGameState, const CMoveAttemptEvent& evt) {
    NavigationComponent_t* pNavComp = pGameState->GetNavigation(evt.idUnit);

    if(pNavComp == nullptr) {
        return;
    }

    pNavComp->vec3Destination = evt.vec3Position;
    pNavComp->eStatus = ENavigationStatus::PATHING;
    RemoveBlocker(pGameState, evt.idUnit);

    if(SpellCastComponent_t* pSpellCastComp = pGameState->GetSpellCast(evt.idUnit)) {
        pSpellCastComp->optCurrentCast.reset();
    }
    if(BasicAttackComponent_t* pAtkComp = pGameState->GetBasicAttack(evt.idUnit)) {
        pAtkComp->optCurrentAttack.reset();
    }
}
void CNavigationSystem::OnMoveIntention(CServerGameState* pGameState, const CMoveIntentionEvent& evt) {
    NavigationComponent_t* pNavComp = pGameState->GetNavigation(evt.idUnit);

    if(pNavComp == nullptr) {
        return;
    }

    RemoveBlocker(pGameState, evt.idUnit);
}

void CNavigationSystem::OnStop(CServerGameState* pGameState, const CStopEvent& evt) {
    if(NavigationComponent_t* pNavComp = pGameState->GetNavigation(evt.idStop)) {
        // TODO this belongsn't here!
        pNavComp->eStatus = ENavigationStatus::IDLE;
        pNavComp->vec3Destination = Vector3::ZERO;
    }
}

void CNavigationSystem::OnDeath(CServerGameState* pGameState, const CDeathEvent& evt) {
    NavigationComponent_t* pNavComp = pGameState->GetNavigation(evt.idTarget);

    if(pNavComp == nullptr) {
        return;
    }

    pNavComp->eStatus = ENavigationStatus::IDLE;
    pNavComp->vec3Destination = Vector3::ZERO;

    RemoveBlocker(pGameState, evt.idTarget);
}

void CNavigationSystem::OnRespawn(CServerGameState* pGameState, const CRespawnEvent& evt) {
    if (NavigationComponent_t* pNav = pGameState->GetNavigation(evt.idTarget)) {
        pNav->eStatus = ENavigationStatus::IDLE;
    }
}