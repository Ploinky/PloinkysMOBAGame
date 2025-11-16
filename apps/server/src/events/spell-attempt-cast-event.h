#pragma once

#include "game/server-game-state.h"
#include "SpellTargetInfo.h"
#include "components/Components.h"
#include "components/Components.h"

class CSpellAttemptCastEvent : public IGameEvent {
public:
    CSpellAttemptCastEvent(UnitId idCaster, SpellTargetInfo info, int nIndex) : m_idCaster(idCaster), m_targetInfo(info), m_nIndex(nIndex) {};

    virtual std::type_index GetType() const override { return typeid(CSpellAttemptCastEvent); };

    UnitId m_idCaster;
    SpellTargetInfo m_targetInfo;
    int m_nIndex;
};