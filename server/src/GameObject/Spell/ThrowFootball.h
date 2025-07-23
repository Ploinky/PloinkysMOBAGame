#include "../Spell.h"

class CThrowFootball : public ISpell {
public:
    virtual void OnCastStart(CSpellCastContext* ctx) override;
    virtual void OnCast(CSpellCastContext* ctx) override;
};