#include <components/Components.h>
#include <cmath>
#include "events/spell-cast-event.h"

CSpellCastComponent::CSpellCastComponent(std::vector<SpellSlot_t> vecSpells) {
    vecSpellSlots = vecSpells;
}