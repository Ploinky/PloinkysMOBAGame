#pragma once

#include "GameObject.h"
#include "GameState.h"
#include "spell-cast-api.h"

enum class ETargetingType {
    UNIT,
    UNIT_INSTANT,
    POINT
};

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

class ISpell {
public:
    ISpell(std::string idSpell) : idSpell(idSpell) {};
    virtual void OnCastStart(CSpellCastApi api, CSpellCastContext* ctx) {};
    virtual void OnCast(CSpellCastApi api, CSpellCastContext* ctx) {};
    virtual void ApplyEffects(CSpellCastApi api, CSpellCastContext* ctx) {};
    
    float fCastPoint = 1000;
    float fCastTime = 2000;
    float fCooldown = 5000;
    float fCastRange = 500;
    std::string idSpell;
    ETargetingType eTargetType = ETargetingType::UNIT_INSTANT;
};