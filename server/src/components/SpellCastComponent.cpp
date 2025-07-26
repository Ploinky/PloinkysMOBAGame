#include <components/Components.h>
#include <cmath>
#include "EventManager.h"
#include "events/spell-cast-event.h"

CSpellCastComponent::CSpellCastComponent(std::vector<SpellSlot_t> vecSpells) {
    m_vecSpells = vecSpells;
}

void CSpellCastComponent::CastSpell(CSpellCastContext* spellCtx) {
    Logger::FormatMsg("casting spell no. %d", spellCtx->nSpellIndex);

    if(spellCtx->nSpellIndex >= m_vecSpells.size()) {
        // TODO ?
        Logger::FormatErr("failed to cast spell %d, incorrect spell index", spellCtx->nSpellIndex);
        return;
    }

    SpellSlot_t& spellSlot = m_vecSpells.at(spellCtx->nSpellIndex);
    
    if(spellSlot.pSpell == nullptr) {
        Logger::FormatErr("failed to cast spell %d, invalid spell pointer", spellCtx->nSpellIndex);
        return;
    }

    if(spellSlot.bIsCasting) {
        Logger::FormatErr("failed to cast spell %d, already casting", spellCtx->nSpellIndex);
        return;
    }

    // TODO stop movement
    m_pGameObject->GetComponent<CNavigationComponent>()->StopNavigation();
    m_pGameObject->GetComponent<CMovementComponent>()->ClearTarget();

    m_vecSpells[spellCtx->nSpellIndex].spellCtx = spellCtx;
    spellSlot.pSpell->OnCastStart(spellCtx);

    m_vecSpells[spellCtx->nSpellIndex].bIsCasting = true;
    m_vecSpells[spellCtx->nSpellIndex].fTimeSinceCast = 0.0f;

    Logger::FormatMsg("Starting spell cast");
    SpellCastStartedData_t data { m_pGameObject->GetId(), spellCtx->nSpellIndex };

    CEventManager::Emit(EEventType::SPELL_CAST_STARTED, &data);
}

std::vector<SpellSlot_t>& CSpellCastComponent::GetSpellSlots() {
    return m_vecSpells;
}