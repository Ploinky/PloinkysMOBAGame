#pragma once 

#include <string>
#include <list>
#include <functional>
#include <Common/PMG_Common.h>
#include "Common/pmg_networking.h"

#include "steam/isteamnetworkingsockets.h"
#include "steam/steam_api.h"

#include "NetworkPeer.h"

// NOTES:
// - api should be agnostic of networking solution (haha good one)
// - should hide complexity and offer high level functions like "send message to client identified by id"

// TODO Abstraction when and where?
typedef CSteamID PlayerID;
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

    // Steam stuff?
    STEAM_GAMESERVER_CALLBACK(ServerNetworkManager, OnConnectionStatusChanged, SteamNetConnectionStatusChangedCallback_t);

    // TODO there has to be a better way
    std::list<NetworkPeer*> GetConnections() {
        return clients_;
    };

private:
    std::list<NetworkPeer*> clients_;

    HSteamListenSocket listenSocket_;
    
    HSteamNetConnection GetConnectionForPlayer(PlayerID playerId);
};