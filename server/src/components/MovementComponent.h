#pragma once

#include "Common/PMG_Common.h"
#include "Components.h"

class CMovementComponent : public IComponent {
public:
    Vector3 GetTarget() const;
    void SetTarget(Vector3 vec3Target);
    void ClearTarget();

    virtual void Update(CGameState* pGameState, float fDelta) override;

private:
    Vector3 m_vec3Target;

};