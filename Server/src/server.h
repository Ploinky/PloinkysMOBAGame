#pragma once 

#include <string>
#include <list>
#include "networking.h"
#include "pmg_networking.h"

namespace PMG {
    class ClientNetworkManager;
    class Game;

    class Server {
    public:
        void Start();
    private:
        ClientNetworkManager* m_networkManager;
        Game* m_game;

        long long GetSystemTime();

        void OnClientConnected(unsigned long id);
        void OnClientDisconnected(unsigned long id);
        void OnMessageReceived(unsigned long clientId, std::vector<uint8_t>* packet);
        void SendMessageToClient(unsigned long clientId, std::vector<uint8_t>* packet);
        void SendMessageToAllClients(std::vector<uint8_t>* packet);
    };
}