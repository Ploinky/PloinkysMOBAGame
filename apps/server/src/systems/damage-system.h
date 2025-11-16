#pragma once

#include "system.h"

class CDamageEvent;
class CHealEvent;

class CDamageSystem : public ISystem {
public:
    CDamageSystem();

    virtual void Update(CServerGameState* pGameState, float fDelta) override;
    virtual void Finalize(CServerGameState* pGameState) override;
    void OnUnitDamaged(CServerGameState* pGameState, CDamageEvent* pEvt);
    void OnUnitHealed(CServerGameState* pGameState, CHealEvent* pEvt);
};