#include "spell-cast-api.h"

#include "GameObject.h"
#include "components/Components.h"
#include "events.h"
#include "game/server-game-state.h"

CSpellCastApi::CSpellCastApi(CServerGameState* pGameState) {
    m_pGameState = pGameState;
}

void CSpellCastApi::ApplyDamage(UnitId idSource, UnitId idTarget, float fDamage) {
    CGameObject* pTarget = m_pGameState->FindGameObjectById(idTarget);
    m_pGameState->VecEvent.emplace(new CDamageEvent(idSource, idTarget, fDamage));
}

void CSpellCastApi::ApplyHeal(UnitId idSource, UnitId idTarget, float fDamage) {
    CGameObject* pTarget = m_pGameState->FindGameObjectById(idTarget);
    m_pGameState->VecEvent.emplace(new CHealEvent(idSource, idTarget, fDamage));
}