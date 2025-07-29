#include "ThrowFootball.h"
#include <Common/logger.h>
#include "GameState.h"
#include "events/damage-event.h"

CThrowFootball::CThrowFootball() {
    fCastPoint = 272;
    fCastTime = 272;
}
void CThrowFootball::OnCastStart(CSpellCastContext* ctx) {
    Logger::Msg("Casting throwfootball");
}

void CThrowFootball::OnCast(CSpellCastContext* ctx) {
    Logger::FormatMsg("Cast of throwfootball completed by %d", ctx->idCaster);
}

void CThrowFootball::ApplyEffects(CSpellCastContext* ctx) {
    Logger::FormatMsg("Applying effects of throwfootball by %d", ctx->idCaster);
    CDamageEvent* ev = new CDamageEvent(ctx->idCaster, ctx->idTarget, 60);
    ctx->EmitEvent(ev);
}