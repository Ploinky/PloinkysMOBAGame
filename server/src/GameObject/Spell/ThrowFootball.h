#include "../Spell.h"

class CThrowFootball : public ISpell {
public:
    CThrowFootball();
    virtual void OnCastStart(CSpellCastContext* ctx) override;
    virtual void OnCast(CSpellCastContext* ctx) override;
    virtual void ApplyEffects(CSpellCastContext* ctx) override;
};