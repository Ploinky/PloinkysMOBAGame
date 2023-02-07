#include "networking.h"

namespace PMG {
  bool Net_Init() {
    WSADATA wsaData = {};

    int wsaStartupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if(wsaStartupResult != 0) {
      return false;
    }

    return true;
  }
  
  bool Net_ConnectToServer(std::string serverAddress, std::string port, net_client_t* client) {
    ADDRINFOA addrinfo = {};
    addrinfo.ai_family = AF_INET;
    addrinfo.ai_socktype = SOCK_STREAM;
    addrinfo.ai_protocol = IPPROTO_TCP;

    ADDRINFOA* addrResult = 0;

    int result = getaddrinfo(serverAddress.c_str(), port.c_str(), &addrinfo, &addrResult);

    if(result != 0) {
      return false;
    }

    SOCKET clientSocket = INVALID_SOCKET;

    clientSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);

    if(clientSocket == INVALID_SOCKET) {
      return false;
    }

    result = connect(clientSocket, addrResult->ai_addr, addrResult->ai_addrlen);

    if(result == SOCKET_ERROR) {
      return false;
    }

    freeaddrinfo(addrResult);

    unsigned long mode = 1;

    result = ioctlsocket(clientSocket, FIONBIO, &mode);

    if (result == SOCKET_ERROR) {
        return false;
    }

    client->socket = clientSocket;
    client->isConnected = true;

    return true;
  }

  
  bool Net_CreateListenSocket(std::string port, net_client_t* listenServer) {
    ADDRINFOA hints = {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    ADDRINFOA* addrResult = 0;

    int error = getaddrinfo(NULL, port.c_str(), &hints, &addrResult);

    if(error != 0) {
      return false;
    }

    SOCKET listenSocket = INVALID_SOCKET;

    listenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);

    if(listenSocket == INVALID_SOCKET) {
      return false;
    }

    error = bind(listenSocket, addrResult->ai_addr, addrResult->ai_addrlen);

    if(error == SOCKET_ERROR) {
      closesocket(listenSocket);
      return false;
    }

    error = listen(listenSocket, SOMAXCONN);

    if(error == SOCKET_ERROR) {
      return false;
    }

    freeaddrinfo(addrResult);

    listenServer->socket = listenSocket;
    listenServer->isConnected = true;

    return true;
  }

  bool Net_AcceptConnection(net_client_t* listenServer, net_client_t* client) {
    TIMEVAL tv = {};
    tv.tv_usec = 1;

    fd_set set = {};
    set.fd_count = 1;
    set.fd_array[0] = listenServer->socket;

    int socketsReady = select(0, &set, nullptr, nullptr, &tv);

    if(socketsReady == 0 || socketsReady == SOCKET_ERROR) {
      return false;
    }

    SOCKET newClientSocket = INVALID_SOCKET;

    newClientSocket = accept(listenServer->socket, NULL, NULL);

    if(newClientSocket == INVALID_SOCKET) {
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

  bool Net_ReceivePacket(net_client_t* connection, packet_t* packet) {
    int error = recv(connection->socket, (char*) &packet->header, sizeof(packet_header_t), 0);

    if(error < 1) {
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

    printf("received packet id <%d> with <%d> bytes from <%d>\r\n",
        packet->header.type,
        packet->size(),
        connection->socket
    );

    return true;
  }

  bool Net_SendPacket(packet_t* packet, net_client_t* connection) {
    int sendBufLen = packet->size();
    char* sendBuf = (char*)std::malloc(sendBufLen);

    if (sendBuf == 0) {
        return false;
    }

    std::memcpy(sendBuf, &packet->header, sizeof(packet_header_t));
    std::memcpy(&sendBuf[sizeof(packet_header_t)], packet->data.data(), packet->size() - sizeof(packet_header_t));
    int error = send(connection->socket, sendBuf, sendBufLen, 0);

    if(error < 1) {
        printf("failed sending <%d> with <%I64u> bytes to <%I64u>: %d\r\n",
            packet->header.type,
            packet->size(),
            connection->socket,
            WSAGetLastError()
        );
        free(sendBuf);
        return false;
    }

    printf("finished sending <%d> with <%I64u> bytes to <%I64u>\r\n",
        packet->header.type,
        packet->size(),
        connection->socket
    );

    free(sendBuf);

    return true;
  }

  bool Net_CloseConnection(net_client_t* connection) {
    int result = shutdown(connection->socket, SD_BOTH);

    if(result == SOCKET_ERROR) {
      return false;
    }

    return true;
  }

  
  bool Net_IsConnected(net_client_t* connection) {
    return connection->isConnected;
  }

  void Net_Shutdown() {
    WSACleanup();
  }
}