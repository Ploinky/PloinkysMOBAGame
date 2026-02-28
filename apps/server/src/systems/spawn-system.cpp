#include "systems/spawn-system.h"

CSpawnSystem::CSpawnSystem(const CGameData* pGameData) : m_pGameData(pGameData) {
}

void CSpawnSystem::Update(CServerGameState* pGameState, float fDelta) {
}

void CSpawnSystem::OnUnitSpawn(CServerGameState* pGameState, CUnitSpawnEvent* pEvt) {
    pGameState->SpawnUnit(*m_pGameData, pEvt->strTemplateId);
}