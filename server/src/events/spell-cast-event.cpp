#include "spell-cast-event.h"

CSpellCastEvent::CSpellCastEvent(CSpellCastContext* spellCtx) {
    m_spellCtx = spellCtx;
}

void CSpellCastEvent::Execute(CGameState* pGameState) {
    CGameObject* pCaster = pGameState->FindGameObjectById(m_spellCtx->idCaster);

    if(pCaster == nullptr) {
        Logger::FormatErr("Invalid spell cast command: missing caster %d", m_spellCtx->idCaster);
        return;
    }
    
    CSpellCastComponent* pCastComp = pCaster->GetComponent<CSpellCastComponent>();

    if(pCastComp == nullptr) {
        Logger::FormatErr("Invalid spell cast command: caster %d is missing spell cast component", m_spellCtx->idCaster);
        return;
    }

    pCastComp->GetSpellSlots()[m_spellCtx->nSpellIndex].pSpell ->OnCast(m_spellCtx);
}