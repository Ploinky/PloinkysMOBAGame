#pragma once

#include "common/pmg_types.h"

class CServerGameState;

class CSpellCastApi {
public:
    CSpellCastApi(CServerGameState* pGameState);

    void ApplyDamage(UnitId idSource, UnitId idTarget, float fDamage);
    void ApplyHeal(UnitId idSource, UnitId idTarget, float fDamage);
private:
    CServerGameState* m_pGameState;
};