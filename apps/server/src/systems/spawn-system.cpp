#include "spawn-system.h"

CSpawnSystem::CSpawnSystem(const CGameData* pGameData) : m_pGameData(pGameData) {
}

void CSpawnSystem::Update(CServerGameState* pGameState, float fDelta) {
}

void CSpawnSystem::OnUnitSpawn(CServerGameState* pGameState, const CUnitSpawnEvent& evt) {
    pGameState->SpawnUnit(*m_pGameData, evt.strTemplateId);
}