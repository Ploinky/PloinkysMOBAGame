#include "network_manager.h"
#include "logger.h"

namespace PMG {
    ClientNetworkManager::ClientNetworkManager() {
    }

    bool ClientNetworkManager::Initialize() {
        WSADATA wsaData = {};

        int wsaStartupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

        if (wsaStartupResult != 0) {
            return false;
        }

        return true;
    }

    bool ClientNetworkManager::CreateListenSocket(std::string port) {
        ADDRINFOA hints = {};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;

        ADDRINFOA* addrResult = 0;

        int error = getaddrinfo(NULL, port.c_str(), &hints, &addrResult);

        if (error != 0) {
            return false;
        }

        SOCKET listenSocket = INVALID_SOCKET;

        listenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);

        if (listenSocket == INVALID_SOCKET) {
            return false;
        }

        error = bind(listenSocket, addrResult->ai_addr, addrResult->ai_addrlen);

        if (error == SOCKET_ERROR) {
            closesocket(listenSocket);
            return false;
        }

        error = listen(listenSocket, SOMAXCONN);

        if (error == SOCKET_ERROR) {
            return false;
        }

        freeaddrinfo(addrResult);

        listen_server_.socket = listenSocket;
        listen_server_.isConnected = true;

        return true;
    }

    bool ClientNetworkManager::AcceptConnection(net_client_t* listenServer, net_client_t* client) {
        TIMEVAL tv = {};
        tv.tv_usec = 1;

        fd_set set = {};
        set.fd_count = 1;
        set.fd_array[0] = listenServer->socket;

        int socketsReady = select(0, &set, nullptr, nullptr, &tv);

        if (socketsReady == 0 || socketsReady == SOCKET_ERROR) {
            return false;
        }

        SOCKET newClientSocket = INVALID_SOCKET;

        newClientSocket = accept(listenServer->socket, NULL, NULL);

        if (newClientSocket == INVALID_SOCKET) {
            return false;
        }

        unsigned long mode = 1;

        int result = ioctlsocket(newClientSocket, FIONBIO, &mode);

        if (result == SOCKET_ERROR) {
            return false;
        }

        client->socket = newClientSocket;
        client->isConnected = true;

        return true;
    }

    bool ClientNetworkManager::Close() {
        int result = shutdown(listen_server_.socket, SD_BOTH);
        listen_server_.isConnected = false;

        if (result == SOCKET_ERROR) {
            return false;
        }

        return true;
    }

    bool ClientNetworkManager::ReceivePacket(net_client_t* connection, packet_t* packet) {
        int error = recv(connection->socket, (char*)&packet->header, sizeof(packet_header_t), 0);

        if (error == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK) {
            connection->isConnected = false;
            Logger::Msg(std::string("Failed to receive message  from <").append(std::to_string(connection->socket)).append(">, error <").append(std::to_string(WSAGetLastError())).append(">"));
            return false;
        } else if (error < 1) {
            return false;
        }

        packet->data.resize(packet->header.size - sizeof(packet_header_t));

        if (packet->header.size > 8) {
            error = recv(connection->socket, (char*)packet->data.data(), packet->header.size - sizeof(packet_header_t), 0);

            if (error < 1) {
                printf("failed receiving <%d> with <%I64u> bytes from <%I64u>: %d\r\n",
                    packet->header.type,
                    packet->size(),
                    connection->socket,
                    WSAGetLastError()
                );
                return false;
            }
        }

        return true;
    }

    void ClientNetworkManager::Update() {
        // Currently allowing new connections at any time
        if (listen_server_.isConnected) {
            net_client_t newClient = {};

            if (AcceptConnection(&listen_server_, &newClient)) {
                clients_.push_back(newClient);
                on_clientConnected(newClient.socket);
            }
        }

        for (auto it = clients_.begin(); it != clients_.end(); ++it) {
            if (!it->isConnected) {
                on_clientDisconnected(it->socket);
                it = clients_.erase(it);
                if (it == clients_.end()) {
                    break;
                }
            }
        }

        for (auto it = clients_.begin(); it != clients_.end(); ++it) {
            packet_t packet = {};

            while (ReceivePacket(&(*it), &packet)) {
                on_clientMessageReceived(it->socket, &packet);
            }
        }
    }

    void ClientNetworkManager::SendToAllClients(packet_t* packet) {
        for (auto it = clients_.begin(); it != clients_.end(); ++it) {
            if(it != clients_.end() && it->isConnected) {
                SendToClient(it->socket, packet);
            }
        }
    }

    void ClientNetworkManager::SendToClient(unsigned long id, packet_t* packet) {
        for (auto it = clients_.begin(); it != clients_.end(); ++it) {
            net_client_t client = *it;

            if (client.socket == id) {
                if(client.isConnected) {
                    size_t sendBufLen = packet->size();
                    char* sendBuf = (char*)std::malloc(sendBufLen);

                    if (sendBuf == 0) {
                        return;  // false;
                    }

                    std::memcpy(sendBuf, &packet->header, sizeof(packet_header_t));
                    std::memcpy(&sendBuf[sizeof(packet_header_t)], packet->data.data(), packet->size() - sizeof(packet_header_t));
                    int error = send(client.socket, sendBuf, sendBufLen, 0);

                    if (error < 1) {
                        printf("failed sending <%d> with <%I64u> bytes to <%I64u>: %d\r\n",
                            packet->header.type,
                            packet->size(),
                            client.socket,
                            WSAGetLastError()
                        );
                        free(sendBuf);
                        
                        // TODO close connection to client?
                        //  Close(&client);
                        return; // false;
                    }

                    free(sendBuf);

                    return; // true;
                }
            }
        }
    }
}