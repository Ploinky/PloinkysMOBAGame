#pragma once

#include "game/client-game-system.h"
#include "client-asset-manager.h"

#include "game/events/events.h"

class CHealthSystem : public ISystem {
public:
    CHealthSystem(CClientAssetManager* pAssetManager);

    virtual void Update(CClientGameState* pGameState, float fDelta) override;

    void OnDamage(CClientGameState* pGameState, CStatsEvent* pEvent);
    void OnDeath(CClientGameState* pGameState, CEntityDeathEvent* pEvent);
private:
    CClientAssetManager* m_pAssetManager;
};