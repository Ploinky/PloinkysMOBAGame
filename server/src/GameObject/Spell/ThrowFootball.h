#include "../Spell.h"

class CThrowFootball : public ISpell {
public:
    virtual void OnCastStart() override;
};