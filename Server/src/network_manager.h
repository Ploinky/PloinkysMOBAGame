#pragma once 

#include <string>
#include <list>
#include <functional>
#include "networking.h"
#include "pmg_networking.h"

namespace PMG {

    class ClientNetworkManager {
    public:
        ClientNetworkManager();

        bool Initialize();
        bool CreateListenSocket(std::string port);

        bool AcceptConnection(net_client_t* listenServer, net_client_t* client);

        bool ReceivePacket(net_client_t* connection, std::vector<uint8_t>* packet);

        void SendToClient(unsigned long clientId, std::vector<uint8_t>* data);
        void SendToClient(unsigned long clientId, Networking::BasePacket* packet);
        void SendToAllClients(std::vector<uint8_t>* packet);

        bool Close();

        void Update();

        std::function<void(unsigned long)> on_clientConnected;
        std::function<void(unsigned long)> on_clientDisconnected;
        std::function<void(unsigned long, std::vector<uint8_t>*)> on_clientMessageReceived;

    private:
        std::list<net_client_t> clients_;
        net_client_t listen_server_{};
    };
}