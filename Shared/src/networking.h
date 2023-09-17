#pragma once

#include <string>
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <vector>

#define DEFAULT_PORT 23119


namespace PMG {
    typedef struct {
        unsigned int socket;
        bool isConnected;
    } net_client_t;
}