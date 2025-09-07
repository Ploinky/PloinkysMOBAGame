#include "particle-system.h"

#include "common/game/game-state.h"
#include "game/components/components.h"
#include "Model.h"

CParticleSystem::CParticleSystem(CClientAssetManager* pAssetManager) {
    m_pAssetManager = pAssetManager;
}

void CParticleSystem::Update(CGameState* pGameState, float fDelta) {
}