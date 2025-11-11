#pragma once

#include "NetworkManager.h"

class LobbyPlayer {
public:
	PlayerID idPlayer;
	bool ready;
	UnitId unit;
	int slot;
};