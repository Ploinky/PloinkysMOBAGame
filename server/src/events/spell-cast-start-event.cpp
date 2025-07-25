#include "spell-cast-start-event.h"

CSpellCastStartEvent::CSpellCastStartEvent(UnitId idCaster, SpellTargetInfo targetInfo, int nIndex) {
    m_idCaster = idCaster;
    m_targetInfo = targetInfo;
    m_nIndex = nIndex;
}

void CSpellCastStartEvent::Execute(CGameState* pGameState) {
    CGameObject* pCaster = pGameState->FindGameObjectById(m_idCaster);

    if(pCaster == nullptr) {
        Logger::FormatErr("Invalid cast start command: unable to find caster %d", m_idCaster);
        return;
    }

    CSpellCastComponent* pSpellCast = pCaster->GetComponent<CSpellCastComponent>();

    if(pSpellCast == nullptr) {
        Logger::FormatErr("Invalid cast start command: caster %d has no spell cast component", m_idCaster);
        return;
    }

    // TODO check validity?

    if(m_targetInfo.target != UNIT_ID_NONE) {
        // this is a unit targeted spell!
        CGameObject* pTarget = pGameState->FindGameObjectById(m_targetInfo.target);

        if(pTarget == nullptr) {
            Logger::FormatErr("Invalid cast start command: unable to find target %d", m_targetInfo.target);
            return;
        }

        // TODO more validity checks for target unit? targetability? team alignment? ...

        CSpellCastContext* ctx = new CSpellCastContext(pGameState);
        ctx->idCaster = m_idCaster;
        ctx->idTarget = m_targetInfo.target;
        ctx->vec2Target = {m_targetInfo.target_point.x, m_targetInfo.target_point.z};
        ctx->nSpellIndex = m_nIndex;
        pSpellCast->CastSpell(ctx);
        return;
    }

}