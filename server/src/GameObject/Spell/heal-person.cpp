#include "heal-person.h"
#include <Common/logger.h>
#include "GameState.h"
#include "events/heal-event.h"

CHealPerson::CHealPerson() {
    fCastPoint = 272;
    fCastTime = 272;
}
void CHealPerson::OnCastStart(CSpellCastContext* ctx) {
    Logger::Msg("Casting heal person");
}

void CHealPerson::OnCast(CSpellCastContext* ctx) {
    Logger::FormatMsg("Cast of heal person completed by %d", ctx->idCaster);
    CHealEvent* ev = new CHealEvent(ctx->idCaster, ctx->idTarget, 10);
    ctx->EmitEvent(ev);
}