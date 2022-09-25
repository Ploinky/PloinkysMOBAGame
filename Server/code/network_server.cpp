#include <iostream>
#include <list>
#include "network_server.hpp"
#include "util.hpp"
#include "network_client.hpp"

namespace PMG {
    void NetworkServer::Start() {
        WSADATA data;
        WORD ver = MAKEWORD(2, 2);

        int wsOk = WSAStartup(ver, &data);

        if (wsOk != 0) {
            std::cerr << "Failed to initialize winsock" << std::endl;
            return;
        }
        listeningSocket = socket(AF_INET, SOCK_STREAM, 0);

        if (listeningSocket == INVALID_SOCKET) {
            std::cerr << "Failed to create listeningSocket socket" << std::endl;
            return;
        }

        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(23119);
        addr.sin_addr.S_un.S_addr = INADDR_ANY;

        bind(listeningSocket, (sockaddr*)&addr, sizeof(addr));

        listen(listeningSocket, SOMAXCONN);

        unsigned long mode = 1;
        if (ioctlsocket(listeningSocket, FIONBIO, &mode) != 0) {
            std::cerr << "Failed to set accept socket non blocking" << std::endl;
            return;
        }

        m_isOpen = true;
    }


    NetworkClient* NetworkServer::CheckForConnection() {
        sockaddr_in clientAddr;
        int clientSize = sizeof(clientAddr);

        SOCKET clientSocket = accept(listeningSocket, (sockaddr*)&clientAddr, &clientSize);

        if ((int)clientSocket == SOCKET_ERROR) {
            if (WSAGetLastError() == WSAEWOULDBLOCK) {
                return NULL;
            }

            std::cout << "Failed to wait for client connection: " << WSAGetLastError() << std::endl;
            Close();
            return NULL;
        }

        char host[NI_MAXHOST];
        char service[NI_MAXSERV];

        ZeroMemory(host, NI_MAXHOST);
        ZeroMemory(service, NI_MAXSERV);

        if (getnameinfo((sockaddr*)&clientAddr, sizeof(clientAddr), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
            std::cout << host << " connected on port " << service << std::endl;
        }
        else {
            inet_ntop(AF_INET, &clientAddr.sin_addr, host, NI_MAXHOST);
            std::cout << host << " connected on port " << ntohs(clientAddr.sin_port) << std::endl;
        }

        unsigned long mode = 1;
        if (ioctlsocket(clientSocket, FIONBIO, &mode) != 0) {
            std::cerr << "Failed to set socket non blocking" << std::endl;
            return NULL;
        }

        NetworkClient* client = new NetworkClient(clientSocket);
        client->id = g_networkClientId++;

        return client;
    }

    void NetworkServer::Close() {
        closesocket(listeningSocket);
    }

    bool NetworkServer::IsOpen() {
        return m_isOpen;
    }
}