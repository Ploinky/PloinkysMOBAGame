#pragma once

#include "game/client-game-system.h"
#include "game/events/events.h"
#include "client-asset-manager.h"

class CAnimationSystem : public ISystem {
public:
    CAnimationSystem(CClientAssetManager* pAssetManager);

    virtual void Update(CClientGameState* pGameState, float fDelta) override;

    void OnAttackStart(CClientGameState* pGameState, CAttackStartEvent* pEvent);
    void OnSpellCastStart(CClientGameState* pGameState, CSpellCastStartEvent* pEvent);
    void OnEntityRespawn(CClientGameState* pGameState, CEntityRespawnEvent* pEvent);
private:
    CClientAssetManager* m_pAssetManager;

    void PlayAnimation(CClientGameState* pGameState, UnitId idUnit, std::string strAnimationId, bool bLoop);
};