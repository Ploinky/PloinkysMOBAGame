#include "GameObjectAction.h"

#include "GameObject/CGameObject.h"
#include "GameState.h"

void CGameObjectActionLineMissile::Process(CGameState* pGameState, CGameObject* pGameObject, float fDt) {
    MissileComponent_t* pMissile = pGameObject->MissileComponent;

    if(pMissile == nullptr) {
        // TODO only handle missiles
        return;
    }

    float fDistance = (pGameObject->position - pMissile->Origin).Length();
    if(fDistance > pMissile->MaxDistance || CompareFloat(fDistance, pMissile->MaxDistance)) {
        // TODO this object needs to die now
        pGameObject->Destroy();
        Logger::Msg("despawn!!");
        return;
    }

    // i add 2 here because for some reason the resulting vec3Target is sometimes a little bit less than max distance away from the origin ...
    // and i do not feel like investigating!
    Vector3 vec3Target = pMissile->Origin + (Vec3TargetPoint - pMissile->Origin).ScaleToLength(pMissile->MaxDistance + 2); // TODO

    pGameObject->StartPlannedMovement(vec3Target, false);
}