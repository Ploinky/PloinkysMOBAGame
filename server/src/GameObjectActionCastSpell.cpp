#include "GameObjectAction.h"

#include "GameState.h"

void CGameObjectActionCastSpell::Process(CGameState* pGameState, CGameObject* pGameObject, float fDt) {
    if(pGameObject->spell_cast_info.current_spell == ISpellIndex) {
        // already casting?
        // TODO
        return;
    }

    pGameObject->spell_cast_info.current_spell = ISpellIndex;
    pGameObject->spell_cast_info.cast_time = 0;
    pGameObject->spell_cast_info.IdTarget = target_info->target;
    pGameObject->spell_cast_info.Vec3Target = target_info->target_point;

    pGameObject->StopPlannedMovement();

    pGameObject->BasicAttackInfo.attack_started = false;
    pGameObject->BasicAttackInfo.sinceAttackStarted = 0;
    pGameObject->BasicAttackInfo.IdTarget = UNIT_ID_NONE;
}