#include <components/Components.h>
#include <cmath>
#include "events/spell-cast-event.h"

SpellCastComponent_t::SpellCastComponent_t(std::vector<SpellSlot_t> vecSpells) {
    vecSpellSlots = vecSpells;
}