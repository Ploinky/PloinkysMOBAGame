#include "heal-event.h"

CHealEvent::CHealEvent(UnitId idSource, UnitId idTarget, int nHeal) {
    m_idSource = idSource;
    m_idTarget = idTarget;
    m_nHeal = nHeal;
}
