#include "ThrowFootball.h"
#include <Common/logger.h>
#include "GameState.h"
#include "../event/damage-event.h"

void CThrowFootball::OnCastStart(CSpellCastContext* ctx) {
    Logger::Msg("Casting throwfootball");
}

void CThrowFootball::OnCast(CSpellCastContext* ctx) {
    Logger::FormatMsg("Cast of throwfootball completed by %d", ctx->idCaster);
    CDamageCommand* ev = new CDamageCommand(ctx->idCaster, ctx->idTarget, 10);
    ctx->EmitEvent(ev);
}