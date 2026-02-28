#pragma once

#include "common/game/game-system.h"
#include "client-asset-manager.h"

#include "game/events/spell-hit-event.h"

class CClientGameState;

class CParticleSystem : public IGameSystem<CClientGameState> {
public:
    CParticleSystem(CClientAssetManager* pAssetManager);

    virtual void Update(CClientGameState* pGameState, float fDelta) override;

    REGISTER_EVENT_HANDLER(CParticleSystem, CSpellHitEvent, OnSpellHit)
private:
    CClientAssetManager* m_pAssetManager;
};