#pragma once

#include "NetworkManagerEnet.h"

class LobbyPlayer {
public:
	PlayerID idPlayer;
	bool ready;
	UnitId unit;
	int slot;
};