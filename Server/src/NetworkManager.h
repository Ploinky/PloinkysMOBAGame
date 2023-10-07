#pragma once 

#include <string>
#include <list>
#include <functional>
#include "networking.h"
#include "pmg_networking.h"

#include "steam/isteamnetworkingsockets.h"
namespace PMG {

    class ServerNetworkManager {
    public:
        ServerNetworkManager();
        ~ServerNetworkManager();

        bool Initialize();
        bool CreateListenSocket(std::string port);

        bool ReceivePacket(HSteamNetConnection conn, std::vector<uint8_t>* packet);

        void SendToClient(HSteamNetConnection conn, std::vector<uint8_t>* data);
        void SendToClient(HSteamNetConnection conn, Networking::BasePacket* packet);
        void SendToAllClients(std::vector<uint8_t>* packet);

        bool Close();

        void Update();

        std::function<void(unsigned long)> on_clientConnected;
        std::function<void(unsigned long)> on_clientDisconnected;
        std::function<void(HSteamNetConnection, std::vector<uint8_t>*)> on_clientMessageReceived;


        STEAM_GAMESERVER_CALLBACK(ServerNetworkManager, OnConnectionStatusChanged, SteamNetConnectionStatusChangedCallback_t);

    private:
        std::list<HSteamNetConnection> clients_;
        net_client_t listen_server_{};

        HSteamListenSocket listenSocket_;

    };
}