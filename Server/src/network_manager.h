#pragma once 

#include <string>
#include <list>
#include <functional>
#include "networking.h"
#include "steam/steam_api.h"

namespace PMG {
    typedef struct {
        HSteamNetConnection socket;
        bool isConnected;
    } net_client_t;

    class NetworkManager {
    public:
        NetworkManager();

        bool Initialize();
        bool CreateListenSocket();

        bool AcceptConnection(HSteamNetConnection connection);
        STEAM_CALLBACK(NetworkManager, OnConnectionStatusChanged, SteamNetConnectionStatusChangedCallback_t);
        STEAM_CALLBACK(NetworkManager, OnSteamNetAuthenticationStatus, SteamNetAuthenticationStatus_t);

        void SendToClient(unsigned long clientId, packet_t* packet);
        void SendToAllClients(packet_t* packet);

        bool Close();

        void Update();

        std::function<void(unsigned long)> on_clientConnected;
        std::function<void(unsigned long)> on_clientDisconnected;
        std::function<void(unsigned long, packet_t*)> on_clientMessageReceived;

    private:
        HSteamListenSocket listen_socket_;
        std::list<net_client_t> clients_;
    };
}