#include <steam/steam_api.h>

typedef CSteamID PlayerID;

class NetworkPeer {
public:
	PlayerID idPlayer;
	HSteamNetConnection pConnection;
};