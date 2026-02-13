#include "trigger-system.h"

#include "game/trigger.h"
static float retard = 5000;

void CTriggerSystem::Update(CServerGameState* pGameState, float fDelta) {
    for(CTrigger trigger : pGameState->VecTriggers) {
        for(CSpawnUnitTrigger spawnUnitTrigger : trigger.vecSpawnUnitTriggers) {
            pGameState->SpawnUnit(*pGameState->m_pGameData, spawnUnitTrigger.idUnitType);
        }
    }
}