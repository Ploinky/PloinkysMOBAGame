#pragma once 

#include <string>
#include <list>
#include "networking.h"

namespace PMG {
    class NetworkManager;
    class Game;

    class Server {
    public:
        void Start();
    private:
        NetworkManager* m_networkManager;
        Game* m_game;

        long long GetSystemTime();

        void OnClientConnected(unsigned long id);
        void OnClientDisconnected(unsigned long id);
        void OnMessageReceived(unsigned long clientId, packet_t* packet);
        void BroadcastMessage(std::vector<packet_t> packet);
        void SendMessageToClient(unsigned long clientId, packet_t* packet);
        void SendMessageToAllClients(packet_t* packet);
    };
}