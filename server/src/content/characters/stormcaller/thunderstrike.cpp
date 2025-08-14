#include "thunderstrike.h"

CThunderstrike::CThunderstrike() : ISpell("thunderstrike") {
    fCastPoint = 230;
    fCastTime = 272;
}

void CThunderstrike::OnCastStart(CSpellCastApi api, CSpellCastContext* ctx) {
}

void CThunderstrike::OnCast(CSpellCastApi api, CSpellCastContext* ctx) {
}

void CThunderstrike::ApplyEffects(CSpellCastApi api, CSpellCastContext* ctx) {
    api.ApplyDamage(ctx->idCaster, ctx->idTarget, 100);
}
