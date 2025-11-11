#include "damage-event.h"
#include "GameObject.h"
#include "components/Components.h"

CDamageEvent::CDamageEvent(UnitId idSource, UnitId idTarget, int nDamage) {
    m_idSource = idSource;
    m_idTarget = idTarget;
    m_nDamage = nDamage;
}
