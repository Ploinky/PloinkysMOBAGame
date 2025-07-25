#pragma once

#include "GameObject.h"
#include "GameState.h"

class CSpellCastContext {
public:
    CSpellCastContext(CGameState* pGameState): m_pGameState(pGameState) {};

    void EmitEvent(IGameEvent* pGameEvent) {
        m_pGameState->VecEvent.push_back(pGameEvent);
    }
    
    UnitId idCaster = UNIT_ID_NONE;
    int nSpellIndex = -1;
    UnitId idTarget = UNIT_ID_NONE;
    Vector2 vec2Target = Vector2(0, 0);
    Vector2 vec2Dir = Vector2(0, 0);

private:
    CGameState* m_pGameState = nullptr;
};

class ISpell {
public:
    virtual void OnCastStart(CSpellCastContext* ctx) = 0;
    virtual void OnCast(CSpellCastContext* ctx) = 0;

    float fCastPoint = 1000;
    float fCastTime = 2000;
    float fCooldown = 5000;
};