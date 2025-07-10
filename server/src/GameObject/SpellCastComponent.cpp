#include <GameObject/Components.h>
#include <cmath>
#include "EventManager.h"

CSpellCastComponent::CSpellCastComponent(std::vector<SpellSlot_t> vecSpells) {
    m_vecSpells = vecSpells;
}

void CSpellCastComponent::Update(CGameState* pGameState, float fDelta) {
    for(int i = 0; i < m_vecSpells.size(); i++) {
        if(m_vecSpells[i].fCooldownRemaining > 0.0f) {
            m_vecSpells[i].fCooldownRemaining = std::max(m_vecSpells[i].fCooldownRemaining - fDelta, 0.0f);
        }

        if(m_vecSpells[i].bIsCasting) {
            m_vecSpells[i].fTimeSinceCast += fDelta;
        }
    }
}

void CSpellCastComponent::CastSpell(int nIndex) {
    Logger::FormatMsg("casting spell no. %d", nIndex);

    if(nIndex >= m_vecSpells.size()) {
        // TODO ?
        Logger::FormatErr("failed to cast spell %d, incorrect spell index", nIndex);
        return;
    }

    SpellSlot_t spellSlot = m_vecSpells.at(nIndex);
    
    if(spellSlot.pSpell == nullptr) {
        Logger::FormatErr("failed to cast spell %d, invalid spell pointer", nIndex);
        return;
    }

    // TODO stop movement
    m_pGameObject->GetComponent<CNavigationComponent>()->StopNavigation();
    m_pGameObject->GetComponent<CMovementComponent>()->ClearTarget();

    spellSlot.pSpell->OnCastStart();
    m_vecSpells[nIndex].bIsCasting = true;
    m_vecSpells[nIndex].fTimeSinceCast = 0.0f;

    SpellCastStartedData_t data { m_pGameObject->GetId(), nIndex };
    CEventManager::Emit(EEventType::SPELL_CAST_STARTED, &data);
}

std::vector<SpellSlot_t> CSpellCastComponent::GetSpellSlots() {
    return m_vecSpells;
}