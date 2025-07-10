#include "GameObjectAction.h"

#include "GameState.h"

void CGameObjectActionAttackUnit::Process(CGameState* pGameState, CGameObject* pGameObject, float fDt) {
    if(pGameObject->BasicAttackInfo.attack_started && pGameObject->BasicAttackInfo.IdTarget == IdTarget) {
        // attack is already in progress, we let it keep going
        return;
    }

    // stop spellcasting
    pGameObject->spell_cast_info.current_spell = -1;
    pGameObject->spell_cast_info.cast_time = 0;
    pGameObject->spell_cast_info.IdTarget = UNIT_ID_NONE;


    CGameObject* pTarget = pGameState->GameObjects.find(IdTarget)->second;
    if((pTarget->position - pGameObject->position).Length() > pGameObject->BasicAttackInfo.range) {
        // not in range, run towards target
        pGameObject->StartPlannedMovement(pTarget->position, true);
        return;
    }

    // we're in range, stop moving
    pGameObject->StopPlannedMovement();

    // TODO cancel old attack?
    pGameObject->BasicAttackInfo.attack_started = true;
    pGameObject->BasicAttackInfo.sinceAttackStarted = 0;
    pGameObject->BasicAttackInfo.IdTarget = IdTarget;
}