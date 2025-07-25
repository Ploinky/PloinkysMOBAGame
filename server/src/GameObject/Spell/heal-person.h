#include "../Spell.h"

class CHealPerson : public ISpell {
public:
    CHealPerson();
    virtual void OnCastStart(CSpellCastContext* ctx) override;
    virtual void OnCast(CSpellCastContext* ctx) override;
};