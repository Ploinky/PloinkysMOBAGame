#include "GameObjectAction.h"

#include "GameObject/CGameObject.h"
#include "GameState.h"

void CGameObjectActionMove::Process(CGameState* pGameState, CGameObject* pGameObject, float fDt) {
    pGameObject->StartPlannedMovement(Vec3TargetPoint, true);
}