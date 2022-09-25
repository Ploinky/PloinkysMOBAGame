#pragma once 

#include <ws2tcpip.h>
#include <string>
#include <list>

namespace PMG {
    extern unsigned long g_networkClientId;

    class NetworkClient {
    public:
        unsigned long id;

        NetworkClient(SOCKET clientSocket);

        void Close();

        void SendNetworkMessage(std::string message);
        void ReceiveNetworkMessages();

        bool IsConnected();

        bool HasNetworkMessage();
        std::string GetNetworkMessage();

        void Update();
    private:
        SOCKET clientSocket;
        std::list<std::string> messages;

        bool m_isConnected;
    };
}