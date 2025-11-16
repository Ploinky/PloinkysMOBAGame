#pragma once

#include "game/client-game-system.h"
#include "client-asset-manager.h"

class CAnimationSystem : public ISystem {
public:
    CAnimationSystem(CClientAssetManager* pAssetManager);

    virtual void Update(CClientGameState* pGameState, float fDelta) override;

private:
    CClientAssetManager* m_pAssetManager;
};