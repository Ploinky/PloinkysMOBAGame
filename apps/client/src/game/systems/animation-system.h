#pragma once

#include <common/game/game-system.h>

#include "../events/events.h"
#include "../../client-asset-manager.h"

class CClientGameState;

class CAnimationSystem : public IGameSystem<CClientGameState> {
public:
    CAnimationSystem(CClientAssetManager* pAssetManager);

    virtual void Update(CClientGameState* pGameState, float fDelta) override;

    REGISTER_EVENT_HANDLER(CAnimationSystem, CAttackFinishedEvent, OnAttackFinished)
    REGISTER_EVENT_HANDLER(CAnimationSystem, CAttackStartEvent, OnAttackStart)
    REGISTER_EVENT_HANDLER(CAnimationSystem, CSpellCastStartEvent, OnSpellCastStart)
    REGISTER_EVENT_HANDLER(CAnimationSystem, CEntityRespawnEvent, OnEntityRespawn)
    REGISTER_EVENT_HANDLER(CAnimationSystem, CEntityDeathEvent, OnEntityDeath)
private:
    CClientAssetManager* m_pAssetManager;

    void PlayAnimation(CClientGameState* pGameState, UnitId idUnit, std::string strAnimationId, bool bLoop);
};