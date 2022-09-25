#pragma once 

#include <string>
#include <list>

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
        void OnMessageReceived(unsigned long clientId, std::string msg);
        void BroadcastMessage(std::string msg);
        void SendMessageToClient(unsigned long clientId, std::string msg);
    };
}