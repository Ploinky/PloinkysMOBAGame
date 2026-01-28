#pragma once

#include <string>


#include "common/pmg_types.h"
#include "common/pmg_physics.h"

class CServerGameState;

class CSpellCastApi {
public:
    CSpellCastApi(CServerGameState* pGameState);

    void ApplyDamage(UnitId idSource, UnitId idTarget, float fDamage);
    void ApplyHeal(UnitId idSource, UnitId idTarget, float fDamage);
    void SpawnEntity(UnitId idSpawner, std::string strTemplateId, Vector2 vec2Destination);
private:
    CServerGameState* m_pGameState;
};