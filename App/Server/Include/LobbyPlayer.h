#pragma once

#include "steam/steam_api.h"

namespace PMG {
	class LobbyPlayer {
	public:
		CSteamID steamId;
		bool ready;
		UnitId unit;
		int slot;
	};
}