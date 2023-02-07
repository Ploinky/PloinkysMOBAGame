#pragma once 

#include <string>
#include <list>
#include <functional>
#include "networking.h"

namespace PMG {
    class NetworkManager {
    public:
        NetworkManager();

        void Host();

        void SendToClient(unsigned long clientId, packet_t* packet);
        void SendToAllClients(packet_t* packet);

        void Close();

        void Update();

        std::function<void(unsigned long)> on_clientConnected;
        std::function<void(unsigned long)> on_clientDisconnected;
        std::function<void(unsigned long, packet_t*)> on_clientMessageReceived;

    private:
        net_client_t m_networkServer;
        std::list<net_client_t> clients;

        bool m_isHosting;
    };
}