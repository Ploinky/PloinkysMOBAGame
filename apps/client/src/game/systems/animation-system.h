#pragma once

#include "common/game/game-system.h"
#include "client-asset-manager.h"

class CAnimationSystem : public IGameSystem {
public:
    CAnimationSystem(CClientAssetManager* pAssetManager);

    virtual void Update(CGameState* pGameState, float fDelta) override;

private:
    CClientAssetManager* m_pAssetManager;
};