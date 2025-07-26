#pragma once

#include "GameObject.h"

class CNavigationComponent : public IComponent {
public:
    CNavigationComponent();

    void NavigateTo(Vector3 vec3Destination);
    void StopNavigation();
	virtual void SetGameObject(CGameObject* pGameObject) override;

    Vector3 m_vec3Destination;
    NavigationGridAgent* m_pNavGridAgent;
    bool m_bIsNavigating;
};