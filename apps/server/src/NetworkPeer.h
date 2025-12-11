#pragma once

typedef struct _ENetHost ENetHost;
typedef uint64_t PlayerID;

class NetworkPeer {
public:
	PlayerID idPlayer;
	ENetHost* pConnection;
};