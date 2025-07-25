#pragma once

#include "GameState.h"
#include "SpellTargetInfo.h"
#include "components/SpellCastComponent.h"

class CSpellCastStartEvent : public IGameEvent {
public:
    CSpellCastStartEvent(UnitId idCaster, SpellTargetInfo info, int nIndex);

    virtual void Execute(CGameState* pGameState) override;

    UnitId m_idCaster;
    SpellTargetInfo m_targetInfo;
    int m_nIndex;
};