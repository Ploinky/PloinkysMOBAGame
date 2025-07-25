#include "GameObject.h"

static UnitId s_idNext = 0;

CGameObject::CGameObject() {
    m_idUnit = s_idNext++;
}

UnitId CGameObject::GetId() {
    return m_idUnit;
}

void CGameObject::Update(CGameState* pGameState, float fDelta) {
    for(IComponent* pComponent : m_vecComponents) {
        pComponent->Update(pGameState, fDelta);
    }
}

void CGameObject::AddComponent(IComponent* pComponent) {
    if(pComponent == nullptr) {
        // TODO this seem overzealous
        return;
    }
    pComponent->SetGameObject(this);
    m_vecComponents.push_back(pComponent);
}

void IComponent::SetGameObject(CGameObject* pGameObject) {
    m_pGameObject = pGameObject;
}
