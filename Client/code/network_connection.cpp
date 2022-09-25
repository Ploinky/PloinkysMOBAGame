#include "network_connection.hpp"
#include "Shared/code/logger.hpp"
#include "Shared/code/util.hpp"

namespace PMG {
    void NetworkConnection::Connect(std::string ip) {
        Logger::Msg("Connecting to network server...");
        
        std::string ipAddress = ip;
        printf("Network server: %s\r\n", ip.c_str());
        int port = 23119;

        WSADATA data;
        WORD version = MAKEWORD(2, 2);
        int wsOk = WSAStartup(version, &data);

        if(wsOk != 0) {
            Logger::Err("Failed to start winsock!");
            return;
        }

        sock = socket(AF_INET, SOCK_STREAM, 0);

        if(sock == INVALID_SOCKET) {
            std::cerr << "Failed to create socket: " << WSAGetLastError() << std::endl;
            WSACleanup();
            return;
        }

        sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(port);
        inet_pton(AF_INET, ipAddress.c_str(), &server.sin_addr);


        int connOk = connect(sock, (sockaddr*)&server, sizeof(server));
        if(connOk == SOCKET_ERROR) {
            std::cerr << "Failed to connect to server: " << WSAGetLastError() << std::endl;
            closesocket(sock);
            WSACleanup();
            return;
        }

        unsigned long mode = 1;
        if (ioctlsocket(sock, FIONBIO, &mode) != 0) {
            std::cerr << "Failed to set accept socket non blocking" << std::endl;
            return;
        }
    }

    void NetworkConnection::Close() {
        WSACleanup();
    }

    void NetworkConnection::ReceiveMessages() {
        char buf[4096];

        ZeroMemory(buf, 4096);
        int bytesReceived = recv(sock, buf, 4096, 0);
        int bytesParsed = 0;
        int msgs = 0;

        while (bytesReceived > 0) {
            // What the hack
            std::string msg(&buf[bytesParsed], 0, bytesReceived);
            messages.push_back(msg);

            int msgLength = msg.length() + 1;
            bytesReceived -= msgLength;
            bytesParsed += msgLength;
        }
    }

    void NetworkConnection::WriteMessage(std::string message) {
        int sendResult = send(sock, message.c_str(), message.size() + 1, 0);

        if(sendResult == SOCKET_ERROR) {
            Close();
        }
    }

    bool NetworkConnection::HasMessage() {
        return messages.size() > 0;
    }

    std::string NetworkConnection::NextMessage() {
        std::string msg = messages.front();
        messages.pop_front();
        return msg;
    }
}