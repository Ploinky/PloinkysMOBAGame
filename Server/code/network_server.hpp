#pragma once 

#include <ws2tcpip.h>
#include <string>
#include <list>

namespace PMG {
    class NetworkClient;

    class NetworkServer {
    public:
        void Start();
        void Close();

        NetworkClient* CheckForConnection();
        bool IsOpen();
    private:
        SOCKET listeningSocket;
        bool m_isOpen;
    };
}