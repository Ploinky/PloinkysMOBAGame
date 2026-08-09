#pragma once

#include <stdint.h>
#include <steam/steam_api.h>

typedef uint64_t PlayerID;

class NetworkPeer {
public:
	PlayerID idPlayer;
	HSteamNetConnection hConnection;
};