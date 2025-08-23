#pragma once 

#include <string>
#include <list>
#include <functional>
#include <common/PMG_Common.h>
#include "common/pmg_networking.h"

#include "NetworkPeer.h"
#include "enet/enet.h"
#include <queue>
#include <map>

// NOTES:
// - api should be agnostic of networking solution (haha good one)
// - should hide complexity and offer high level functions like "send message to client identified by id"

// TODO Abstraction when and where?
typedef uint64_t PlayerID;

/*
* Manages connections to clients.
*/
class ServerNetworkManager {
public:
    ServerNetworkManager();
    ~ServerNetworkManager();

    // Initialization
    bool Initialize();
    bool CreateListenSocket(std::string port);


    void Update();
    bool ReceivePacket(PlayerID playerId, std::vector<uint8_t>* packet);
    void SendToClient(PlayerID playerId, std::vector<uint8_t>* data);
    void SendToClient(PlayerID playerId, BasePacket* packet);
    void SendToAllClients(std::vector<uint8_t>* packet);
    void SendToAllClients(BasePacket* packet);
    void SendToAllClients(BasePacket& packet);

    // Shutdown
    void StopListenSocket();
    bool Close();

    // Events?
    std::function<void(PlayerID newPlayerId)> on_clientConnected;
    std::function<void(PlayerID oldPlayerId)> on_clientDisconnected;
    std::function<void(PlayerID playerId, std::vector<uint8_t>*)> on_clientMessageReceived;

    // TODO there has to be a better way
    std::list<NetworkPeer*> GetConnections() {
        return clients_;
    };

private:
    std::list<NetworkPeer*> clients_;

    ENetHost* m_pServerSocket;
    
    ENetPeer* GetConnectionForPlayer(PlayerID playerId);

    uint64_t m_uCurrId = 0;
};