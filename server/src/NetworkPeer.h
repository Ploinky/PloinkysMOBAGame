#include "enet/enet.h"

typedef uint64_t PlayerID;

class NetworkPeer {
public:
	PlayerID idPlayer;
	ENetPeer* pConnection;
};