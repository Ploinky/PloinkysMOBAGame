#include <GameObject/Components.h>
#include <cmath>
#include "EventManager.h"

CSpellCastComponent::CSpellCastComponent(std::vector<SpellSlot_t> vecSpells) {
    m_vecSpells = vecSpells;
}

void CSpellCastComponent::Update(CGameState* pGameState, float fDelta) {
    for(int i = 0; i < m_vecSpells.size(); i++) {
        SpellSlot_t& spellSlot = m_vecSpells.at(i);

        if(spellSlot.fCooldownRemaining > 0.0f) {
            spellSlot.fCooldownRemaining = std::max(spellSlot.fCooldownRemaining - fDelta, 0.0f);
            continue;
        }

        if(!spellSlot.bIsCasting) {
            continue;
        }

        if(spellSlot.fTimeSinceCast < spellSlot.pSpell->fCastPoint && spellSlot.fTimeSinceCast + fDelta >= spellSlot.pSpell->fCastPoint) {
            CSpellCastContext ctx(pGameState);
            ctx.idCaster = m_pGameObject->GetId();
            ctx.idTarget = spellSlot.pTargetInfo->target;
            spellSlot.pSpell ->OnCast(&ctx);
        }

        spellSlot.fTimeSinceCast += fDelta;

        if(spellSlot.fTimeSinceCast >= spellSlot.pSpell->fCastTime) {
            spellSlot.fCooldownRemaining = spellSlot.pSpell->fCooldown;
            spellSlot.bIsCasting = false;
            spellSlot.fTimeSinceCast = 0.0f;
        }
    }
}

void CSpellCastComponent::CastSpell(int nIndex, SpellTargetInfo* pTargetInfo) {
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

    CSpellCastContext ctx(nullptr);
    ctx.idCaster = m_pGameObject->GetId();
    ctx.idTarget = pTargetInfo->target;
    spellSlot.pSpell->OnCastStart(&ctx);
    m_vecSpells[nIndex].bIsCasting = true;
    m_vecSpells[nIndex].fTimeSinceCast = 0.0f;
    m_vecSpells[nIndex].pTargetInfo = pTargetInfo;

    SpellCastStartedData_t data { m_pGameObject->GetId(), nIndex };
    CEventManager::Emit(EEventType::SPELL_CAST_STARTED, &data);
}

std::vector<SpellSlot_t> CSpellCastComponent::GetSpellSlots() {
    return m_vecSpells;
}