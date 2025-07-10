#include "GameObjectAction.h"

#include "GameState.h"

void CGameObjectActionStop::Process(CGameState* pGameState, CGameObject* pGameObject, float fDt) {
    pGameObject->StopPlannedMovement();
}