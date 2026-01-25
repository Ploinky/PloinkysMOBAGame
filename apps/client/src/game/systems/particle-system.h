#pragma once

#include "game/client-game-system.h"
#include "client-asset-manager.h"

#include "game/events/spell-hit-event.h"

class CParticleSystem : public ISystem {
public:
    CParticleSystem(CClientAssetManager* pAssetManager);

    virtual void Update(CClientGameState* pGameState, float fDelta) override;

	void OnSpellHit(CClientGameState* pGameState, CSpellHitEvent* pHitEvent);
private:
    CClientAssetManager* m_pAssetManager;
};