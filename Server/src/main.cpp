#include <iostream>
#include "server.h"

int main(int argc, char** argv) {
    std::cout << "Starting Ploinky's MOBA Game Server..." << std::endl;

    PMG::Server* server = new PMG::Server();
    server->Start();

    return 0;
}