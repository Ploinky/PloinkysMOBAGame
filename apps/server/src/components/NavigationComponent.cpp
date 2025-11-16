#include "Components.h"

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