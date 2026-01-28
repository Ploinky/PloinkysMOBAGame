#include "spell-cast-api.h"

#include "GameObject.h"
#include "components/Components.h"
#include "events.h"
#include "game/server-game-state.h"

CSpellCastApi::CSpellCastApi(CServerGameState* pGameState) {
    m_pGameState = pGameState;
}

void CSpellCastApi::ApplyDamage(UnitId idSource, UnitId idTarget, float fDamage) {
    m_pGameState->VecEvent.emplace(new CDamageEvent(idSource, idTarget, fDamage));
}

void CSpellCastApi::ApplyHeal(UnitId idSource, UnitId idTarget, float fDamage) {
    m_pGameState->VecEvent.emplace(new CHealEvent(idSource, idTarget, fDamage));
}

void CSpellCastApi::SpawnEntity(UnitId idSpawner, std::string strTemplateId, Vector2 vec2Destination) {
    CUnitSpawnEvent* pEvt = new CUnitSpawnEvent();
    pEvt->strTemplateId = strTemplateId;
    pEvt->vec2Destination = vec2Destination;
    m_pGameState->VecEvent.emplace(pEvt);
}