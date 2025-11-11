#pragma once

#include "GameObject.h"
#include "GameState.h"
#include "spell-cast-api.h"


class CSpellCastContext {
public:
    CSpellCastContext(CGameState* pGameState): m_pGameState(pGameState) {};

    void EmitEvent(IGameEvent* pGameEvent) {
        m_pGameState->VecEvent.emplace(pGameEvent);
    }
    
    UnitId idCaster = UNIT_ID_NONE;
    int nSpellIndex = -1;
    UnitId idTarget = UNIT_ID_NONE;
    Vector2 vec2Target = Vector2(0, 0);
    Vector2 vec2Dir = Vector2(0, 0);
    
    private:
    CGameState* m_pGameState = nullptr;
};