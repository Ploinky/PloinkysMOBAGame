#pragma once

#include "steam/steam_api.h"

class LobbyPlayer {
public:
	CSteamID steamId;
	bool ready;
	UnitId unit;
	int slot;
};