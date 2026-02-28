#pragma once

#include "common/game/game-system.h"

class CDamageEvent;
class CHealEvent;
class CServerGameState;

class CDamageSystem : public IGameSystem<CServerGameState> {
public:
    CDamageSystem();

    virtual void Update(CServerGameState* pGameState, float fDelta) override;
    virtual void Finalize(CServerGameState* pGameState) override;

    REGISTER_EVENT_HANDLER(CDamageSystem, CDamageEvent, OnUnitDamaged);
    REGISTER_EVENT_HANDLER(CDamageSystem, CHealEvent, OnUnitHealed);
};