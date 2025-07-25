#include "spell-system.h"

#include "GameState.h"
#include "events/spell-cast-start-event.h"

#include "events/damage-event.h"

void CSpellSystem::Process(CGameState* pGameState, float fDelta) {
     std::vector<CSpellCastStartEvent*> vecEvts = pGameState->GetEvents<CSpellCastStartEvent>();

    for(CSpellCastStartEvent* pEvt : vecEvts) {
        Logger::FormatMsg("Received cast start event, processing...");

        CGameObject* pCaster = pGameState->FindGameObjectById(pEvt->m_idCaster);

        if(pCaster == nullptr) {
            Logger::FormatErr("Invalid cast start event: caster with id %d does not exist", pEvt->m_idCaster);
            continue;
        }

        CGameObject* pTarget = pGameState->FindGameObjectById(pEvt->m_targetInfo.target);

        if(pTarget == nullptr) {
            Logger::FormatErr("Invalid cast start event: target with id %d does not exist", pEvt->m_targetInfo.target);
            continue;
        }

        CSpellCastComponent* pSpellComp = pCaster->GetComponent<CSpellCastComponent>();

        if(pSpellComp == nullptr) {
            Logger::FormatErr("Invalid cast start event: caster with id %d does not not have a spell cast component", pEvt->m_idCaster);
            continue;
        }

        CSpellCastContext* pSpellCtx = new CSpellCastContext(pGameState);
        pSpellCtx->idCaster = pEvt->m_idCaster;
        pSpellCtx->idTarget = pEvt->m_targetInfo.target;
        pSpellCtx->nSpellIndex = pEvt->m_nIndex;
        pSpellComp->CastSpell(pSpellCtx);
    }

    for(std::pair<UnitId, CGameObject*> pGameObj : pGameState->GameObjects) {
        CSpellCastComponent* pSpellComp = pGameObj.second->GetComponent<CSpellCastComponent>();

        if(pSpellComp == nullptr) {
            continue;
        }

        std::vector<SpellSlot_t>& vecSpells = pSpellComp->GetSpellSlots();

        for(int i = 0; i < vecSpells.size(); i++) {
            SpellSlot_t& spellSlot = vecSpells.at(i);

            if(spellSlot.fCooldownRemaining > 0.0f) {
                spellSlot.fCooldownRemaining = std::max(spellSlot.fCooldownRemaining - fDelta, 0.0f);
                continue;
            }

            if(!spellSlot.bIsCasting) {
                continue;
            }

            if(spellSlot.fTimeSinceCast < spellSlot.pSpell->fCastPoint && spellSlot.fTimeSinceCast + fDelta >= spellSlot.pSpell->fCastPoint) {
                spellSlot.pSpell->OnCast(spellSlot.spellCtx);
            }

            spellSlot.fTimeSinceCast += fDelta;

            if(spellSlot.fTimeSinceCast >= spellSlot.pSpell->fCastTime) {
                spellSlot.fCooldownRemaining = spellSlot.pSpell->fCooldown;
                spellSlot.bIsCasting = false;
                spellSlot.fTimeSinceCast = 0.0f;
            }
        }
    }
}