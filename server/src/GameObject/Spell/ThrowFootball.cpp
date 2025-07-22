#include "ThrowFootball.h"
#include <Common/logger.h>

void CThrowFootball::OnCastStart() {
    Logger::Msg("Casting throwfootball");
}

void CThrowFootball::OnCast() {
    Logger::Msg("Cast of throwfootball completed");
}