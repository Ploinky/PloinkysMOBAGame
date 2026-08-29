#include "client-game-state.h"

void CClientGameState::DeleteUnit(UnitId id) {
    RemoveAnimation(id);
    RemoveAttack(id);
    RemoveAudioEmitter(id);
    RemoveHealth(id);
    RemoveInventory(id);
    RemoveMovement(id);
    RemoveParticle(id);
    RemovePickupable(id);
    RemoveRenderable(id);
    RemoveSpellCast(id);
    RemoveTargetable(id);
    RemoveTeam(id);
    RemoveTitle(id);
    RemoveTransform(id);
    RemoveUseable(id);
}