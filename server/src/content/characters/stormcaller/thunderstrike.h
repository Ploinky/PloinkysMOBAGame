#pragma once

#include "GameObject/Spell.h"

class CThunderstrike : public ISpell {
public:
    CThunderstrike() : ISpell("thunderstrike") {};
    virtual void OnCastStart(CSpellCastApi api, CSpellCastContext* ctx) override;
    virtual void OnCast(CSpellCastApi api, CSpellCastContext* ctx) override;
    virtual void ApplyEffects(CSpellCastApi api, CSpellCastContext* ctx) override;
};