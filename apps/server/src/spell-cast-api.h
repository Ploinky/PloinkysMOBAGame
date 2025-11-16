#pragma once

#include "GameState.h"

class CSpellCastApi {
public:
    CSpellCastApi(CGameState* pGameState);

    void ApplyDamage(UnitId idSource, UnitId idTarget, float fDamage);

private:
    CGameState* m_pGameState;
};