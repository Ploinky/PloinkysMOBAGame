#include "trigger-system.h"

#include "game/trigger.h"
#include "common/PMG_Common.h"

void CTriggerSystem::Update(CServerGameState* pGameState, float fDelta) {
    float currTime = pGameState->CurrentTick * (1000.0f / 60.0f);
    for(CTrigger& trigger : pGameState->VecTriggers) {
        for(CSpawnUnitTrigger& spawnUnitTrigger : trigger.vecSpawnUnitTriggers) {
            if(spawnUnitTrigger.nLastTime == -1 && currTime >= spawnUnitTrigger.nTime) {
                spawnUnitTrigger.nLastTime = currTime;
                UnitId idSpawnedUnit = pGameState->SpawnUnit(*pGameState->m_pGameData, spawnUnitTrigger.idUnitType);
                pGameState->GetTransform(idSpawnedUnit)->SetPosition({spawnUnitTrigger.vec2Position.x, 0, spawnUnitTrigger.vec2Position.y});
                pGameState->GetNavigation(idSpawnedUnit)->vec3Destination = pGameState->GetTransform(idSpawnedUnit)->GetPosition();
                pGameState->GetMovement(idSpawnedUnit)->vec3Target = pGameState->GetTransform(idSpawnedUnit)->GetPosition();
            }
        }
    }
}