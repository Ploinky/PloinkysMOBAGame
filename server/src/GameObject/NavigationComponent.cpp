#include "Components.h"

#include "GameState.h"

CNavigationComponent::CNavigationComponent() {
    m_vec3Destination = Vector3::ZERO;
    m_pNavGridAgent = nullptr;
    m_bIsNavigating = false;
}

void CNavigationComponent::NavigateTo(Vector3 vec3Destination) {
    m_vec3Destination = vec3Destination;
    m_bIsNavigating = true;
}

void CNavigationComponent::StopNavigation() {
    m_bIsNavigating = false;
    m_vec3Destination = Vector3::ZERO;
    
    if(CMovementComponent* pMovement = m_pGameObject->GetComponent<CMovementComponent>()) {
        pMovement->ClearTarget();
    }
}

void CNavigationComponent::SetGameObject(CGameObject* pGameObject) {
    IComponent::SetGameObject(pGameObject);

    if(m_pNavGridAgent != nullptr) {
        delete m_pNavGridAgent;
    }

    m_pNavGridAgent = new NavigationGridAgent();
    m_pNavGridAgent->UnitId = m_pGameObject->GetId();
    m_pNavGridAgent->IgnoreCollision = false;
}

void CNavigationComponent::Update(CGameState* pGameState, float fDelta) {
    if(!m_bIsNavigating) {
        return;
    }

    NavigationMap* pNavMap = pGameState->GetNavMap();

    CTransformComponent* pTransform = m_pGameObject->GetComponent<CTransformComponent>();
    CMovementComponent* pMovement = m_pGameObject->GetComponent<CMovementComponent>();

    if(pTransform == nullptr || pMovement == nullptr) {
        return;
    }
    
    if((m_vec3Destination - pTransform->GetPosition()).Length() < 10) {
        return;
    }

    if(m_pNavGridAgent->path.size() == 0) {
        m_pNavGridAgent->path = pNavMap->GetPath(m_pNavGridAgent, {pTransform->GetPosition().x, pTransform->GetPosition().z}, {m_vec3Destination.x, m_vec3Destination.z});
        
        if(m_pNavGridAgent->path.size() == 0) {
            // TODO this needs to be handled
            return;
        }
    }

    Vector2 vec2IntermediateTarget = m_pNavGridAgent->path.at(0);
    Vector3 vec3IntermediateTarget = {vec2IntermediateTarget.x, 0, vec2IntermediateTarget.y};

    // TODO magic number
    if((vec3IntermediateTarget - pTransform->GetPosition()).Length() < 10) {
        m_pNavGridAgent->path.erase(m_pNavGridAgent->path.begin());

        if(m_pNavGridAgent->path.size() == 0) {
            return;
        }
        vec2IntermediateTarget = m_pNavGridAgent->path.at(0);
        vec3IntermediateTarget = {vec2IntermediateTarget.x, 0, vec2IntermediateTarget.y};
        pMovement->SetTarget({m_pNavGridAgent->path.at(0).x, 0, m_pNavGridAgent->path.at(0).y});
        return;
    }

    m_pNavGridAgent->path = pNavMap->GetPath(m_pNavGridAgent, {pTransform->GetPosition().x, pTransform->GetPosition().z}, {m_vec3Destination.x, m_vec3Destination.z});
    
    if(m_pNavGridAgent->path.size() == 0) {
        // TODO this needs to be handled
        return;
    }

    pMovement->SetTarget({m_pNavGridAgent->path.at(0).x, 0, m_pNavGridAgent->path.at(0).y});
}