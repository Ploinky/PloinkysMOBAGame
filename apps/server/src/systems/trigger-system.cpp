#include "trigger-system.h"

#include "game/trigger.h"
#include "common/PMG_Common.h"

void CTriggerSystem::Update(CServerGameState* pGameState, float fDelta) {
    float currTime = pGameState->CurrentTick * (1000.0f / 60.0f);
    for(CTrigger& trigger : pGameState->VecTriggers) {
        for(CSpawnUnitTrigger& spawnUnitTrigger : trigger.vecSpawnUnitTriggers) {
            if(spawnUnitTrigger.nLastTime == -1 && currTime >= spawnUnitTrigger.nTime) {
                spawnUnitTrigger.nLastTime = currTime;
                pGameState->SpawnUnit(*pGameState->m_pGameData, spawnUnitTrigger.idUnitType);
            }
        }
    }
}