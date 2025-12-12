#pragma once

typedef struct _ENetPeer ENetPeer;
typedef uint64_t PlayerID;

class NetworkPeer {
public:
	PlayerID idPlayer;
	ENetPeer* pConnection;
};