#include "steam/steam_api.h"

namespace PMG {
	class NetworkPeer {
	public:
		CSteamID steamId;
		HSteamNetConnection connection;
	};
}