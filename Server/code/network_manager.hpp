#pragma once 

#include <string>
#include <list>
#include <functional>

namespace PMG {
    class NetworkServer;
    class NetworkClient;

    class NetworkManager {
    public:
        NetworkManager();

        void Host();

        void SendToClient(unsigned long clientId, std::string msg);
        void SendToAllClients(std::string msg);

        void Close();

        void Update();

        std::function<void(unsigned long)> on_clientConnected;
        std::function<void(unsigned long)> on_clientDisconnected;
        std::function<void(unsigned long, std::string)> on_clientMessageReceived;

    private:
        NetworkServer* m_networkServer;
        std::list<NetworkClient*> clients;

        bool m_isHosting;
    };
}