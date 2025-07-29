#pragma once

#include "system.h"

class CDamageEvent;
class CHealEvent;

class CDamageSystem : public ISystem {
public:
    CDamageSystem();

    virtual void Update(CGameState* pGameState, float fDelta) override;
    virtual void Finalize(CGameState* pGameState) override;
    void OnUnitDamaged(CGameState* pGameState, CDamageEvent* pEvt);
    void OnUnitHealed(CGameState* pGameState, CHealEvent* pEvt);
};