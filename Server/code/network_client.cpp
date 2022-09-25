#include "network_client.hpp"
#include <iostream>
#include "main.hpp"
#include "util.hpp"

namespace PMG {
    unsigned long g_networkClientId;

    NetworkClient::NetworkClient(SOCKET socket) {
        clientSocket = socket;
        m_isConnected = true;
    }

    void NetworkClient::SendNetworkMessage(std::string message) {
        send(clientSocket, message.c_str(), message.size() + 1, 0);
    }

    void NetworkClient::ReceiveNetworkMessages() {
        char buf[4096];
        ZeroMemory(buf, 4096);

        int bytesReceived = recv(clientSocket, buf, 4096, 0);

        if (bytesReceived == 0 ||
            (bytesReceived == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK)) {
            return;
        }

        if (bytesReceived == SOCKET_ERROR) {
            switch (WSAGetLastError()) {
            case WSAECONNRESET: {
                std::cout << "Client disconnected" << std::endl;
                break;
            }
            default: {
                std::cout << "Error receiving data from client, disconnecting" << std::endl;
                break;
            }
            }

            Close();
            return;
        }

        messages.push_back(std::string(buf, 0, bytesReceived));
    }


    void NetworkClient::Close() {
        messages.clear();

        closesocket(clientSocket);

        m_isConnected = false;
    }

    bool NetworkClient::HasNetworkMessage() {
        return !messages.empty();
    }

    std::string NetworkClient::GetNetworkMessage() {
        std::string msg = messages.front();
        messages.pop_front();
        return msg;
    }

    bool NetworkClient::IsConnected() {
        return m_isConnected;
    }

    void NetworkClient::Update() {
        ReceiveNetworkMessages();
    }
}