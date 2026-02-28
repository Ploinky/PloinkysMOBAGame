#pragma once

#include "common/game/game-system.h"
#include "client-asset-manager.h"

#include "game/events/events.h"

class CHealthSystem : public IGameSystem<CClientGameState> {
public:
    CHealthSystem(CClientAssetManager* pAssetManager);

    virtual void Update(CClientGameState* pGameState, float fDelta) override;

    REGISTER_EVENT_HANDLER(CHealthSystem, CStatsEvent, OnDamage)
    REGISTER_EVENT_HANDLER(CHealthSystem, CEntityDeathEvent, OnDeath)
private:
    CClientAssetManager* m_pAssetManager;
};