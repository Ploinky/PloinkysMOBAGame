#pragma once

#include "steam\steam_api.h"

namespace PMG {
	class LobbyPlayer {
	public:
		HSteamNetConnection socket;
		CSteamID steamId;
	};
}