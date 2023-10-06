#pragma once

#include <string>
#include <vector>

#define DEFAULT_PORT 23119


namespace PMG {
    typedef struct {
        unsigned int socket;
        bool isConnected;
    } net_client_t;
}